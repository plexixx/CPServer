#include "cpserver.h"
#include "ui_cpserver.h"
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QIODevice>
#include <QTextCodec>
#include <QByteArray>

QVector<User> aCustomer;

CPServer::CPServer(QWidget *parent)
    : QObject(parent)
    , ui(new Ui::MainWindow)
    , CP(MAX_F_CPNUM + MAX_T_CPNUM +2)
    , report(MAX_F_CPNUM + MAX_T_CPNUM +2)
    , allUser (MAX_USER_NUM + 1)
{
    db = new DB();
    FCallNum = 1;
    TCallNum = 1;
    getEvent();

    //系统时间更新
    //systimer = new SysTimer();
    timer = new QTimer(); //创建定时器
    systime = new QTime(6,13); // 6:00:00，启动后进入到5点55，
    connect(timer, SIGNAL(timeout()), this, SLOT(addSecs()));
    //连接槽函数，将timer的timeout行为，连接到updateTime函数中
    connect(timer, SIGNAL(timeout()), this, SLOT(updateTimeDeal()));

    //开启所有的充电桩
    for (int i=1; i<=MAX_F_CPNUM; i++)
        CP[i].turnOn(i, F_MODE);
    for (int i=MAX_F_CPNUM+1; i<=MAX_T_CPNUM + MAX_F_CPNUM; i++)
        CP[i].turnOn(i, T_MODE);
    qDebug() << "所有充电桩均已开启" << endl;

    timer->start(MS_PER_MIN);


//    connect(this, SIGNAL(signal_startpower()), this, );
//    connect(this, SIGNAL(signal_endpower()), this, );



    tcp_server = new QTcpServer;
    tcp_server->listen(QHostAddress::LocalHost, 6666);
    qDebug()<<("start listen to port 6666 of local host!") << endl;
   // connect(tcp_server, SIGNAL(newConnection()), this, SLOT(onNewConnection()), Qt::QueuedConnection);

    //管理员
    //curmanager = new manager();
    waitarea = new WaitArea();  //等候区
    curUser = new User();



}

void CPServer::updateTimeDeal()
{

    //输入事件处理
    if (systime->minute() % 5 == 0)
    {

        QString tmpTimeStr = QString("%1:%2:00").arg(systime->hour())
                .arg(systime->minute());
        qDebug() << QString("%1 进行事件的判断").arg(tmpTimeStr) << endl;
        if (TimetoEvent[tmpTimeStr].isEmpty() == 0)
        {
            qDebug() << "匹配到事件" << endl;
            QString eventStr = TimetoEvent[tmpTimeStr];
            QString type = eventStr.section(',', 0, 0);
            QString id = eventStr.section(',', 1, 1);
            QString mode = eventStr.section(',', 2, 2);
            float du = eventStr.section(',', 3, 3).toInt();
            EventCome(type, id, mode, du);
        }
    }

    qDebug() << QString("%1:%2 进行状态更新").arg(systime->hour())
                .arg(systime->minute()) << endl;

    //emit signal_endpower();
   // 考虑 等候区的状态
    //考虑叫号和调度进入充电区问题
    //考虑进入等候区的处理问题
    QVector<int> q;
    //先看一下需不需要叫号
    if (waitarea->CurParkNum > 0)
    {

        int userId;  //被叫的用户
        int CPid;   //调度确定的充电桩
        if (FCallNum == 1 && waitarea->FQueue.size() > waitarea->it_F)
        {
            qDebug() << "快充队列有空位，可叫号" << endl;
            if (waitarea->StartPriority == 1)
            {
                userId = waitarea->PriorityCallNum(F_MODE);
            }
            else
                userId = waitarea->CallNum(F_MODE);

                qDebug() << QString("叫完号了===%1======").arg(userId) << endl;
                CPid = sysSchedule(F_MODE);    //叫号后面就是调度
                qDebug() << QString("要放到充电桩%1去").arg(CPid) << endl;
                GotoChargeArea(F_MODE, CPid, userId);
                allUser[userId].prog =CHARGEWAIT;
                qDebug() << QString("用户%1到达快充电桩%2")
                         .arg(userId).arg(CPid)<< endl;

        }
        if (TCallNum == 1 && waitarea->TQueue.size()  > waitarea->it_T)
        {
            if (waitarea->StartPriority == 1)
                userId = waitarea->PriorityCallNum(T_MODE);
            else
                userId = waitarea->CallNum(T_MODE);
            CPid = sysSchedule(T_MODE);
            GotoChargeArea(T_MODE, CPid, userId);
            allUser[userId].prog =CHARGEWAIT;
            allUser[userId].CPid = CPid;
            qDebug() << QString("用户%1到达慢充电桩%2")
                     .arg(userId).arg(CPid)<< endl;
        }
    }
    else{
        qDebug() << "没有人在等候区，无需叫号" << endl;
    }


        //考虑充电桩的状态，以及详单和报表的更新
//        emit signal_startpower();
    bool haveCPFree = 0;    //有充电桩空闲
    for (int i=1; i<=MAX_F_CPNUM; i++)  //遍历所有充电桩
    {
        if (CP[i].state == CP_FREE)   //充电桩处于空闲状态
        {

            if (CP[i].queue.size() == 0)  //该充电桩里没人排队
            {
                qDebug() << QString("充电桩 %1 处于空闲状态且没人排队").arg(i) << endl;
                haveCPFree = 1;
            }
            else    //有人排队，则可以开始工作
            {
                if (CP[i].queue.size() < MAX_CHARGE_QUEUE_LEN)
                    haveCPFree = 1; //还有空位
                qDebug() << QString("充电桩 %1 处于空闲状态但有人排队").arg(i) << endl;
                //充电桩队列里有人排队，则可以开始充电
                //充电桩
                int topUserId = CP[i].queue[0];
                allUser[topUserId].prog = CHARGEPOW;
                CP[i].start(allUser[topUserId].NeedChargeTime);
                // 详单
                Bill bill;
                bill.createBill(i, systime->hour(), systime->minute(), F_MODE);
                allBill.push_back(bill);
                allBill.end()->id = allBill.size()-1;   //设置详单编号
                CPtoBill[i] = allBill.end()->id;
                //报表
//                Report newreport("6-16");
//                CPToReport[i] = newreport;

            }
        }
        else if (CP[i].state == CP_POWERING)    //充电桩处于充电状态
        {
            CP[i].OverPeriodUpdate();   //充电桩进行刷新
            allBill[CPtoBill[i]].updateBill(systime->hour());   //详单进行刷新
            if (CP[i].state == CP_FREE) //由充电状态转为空闲状态，说明充电结束
            {
                allUser[CP[i].queue[0]].prog = CHARGOK;
                CP[i].queue.pop_front();
                 allBill[CPtoBill[i]].finishBill(systime->hour(), systime->minute());
                 report[i].UpdateReport(CP[i], allBill[CPtoBill[i]]);   //得到所有的报表

            }
        }
    }
    FCallNum = haveCPFree;  //只要有一个空位，就可以叫号
    haveCPFree = 0;
    for (int i=MAX_F_CPNUM+1; i<=MAX_F_CPNUM + MAX_T_CPNUM; i++)  //遍历所有充电桩
    {
        if (CP[i].state == CP_FREE)   //充电桩处于空闲状态
        {
            if (CP[i].queue.size() == 0)  //该充电桩里没人排队
            {
               qDebug() << QString("充电桩 %1 处于空闲状态且没人排队").arg(i) << endl;
                haveCPFree = 1;
            }
            else    //有人排队，则可以开始工作
            {
                if (CP[i].queue.size() < MAX_CHARGE_QUEUE_LEN)
                    haveCPFree = 1; //还有空位
                qDebug() << QString("充电桩 %1 处于空闲状态但有人排队").arg(i) << endl;
                //充电桩队列里有人排队，则可以开始充电
                //充电桩
                int topUserId = CP[i].queue[0];
                CP[i].start(allUser[topUserId].NeedChargeTime);
                qDebug() << QString("用户 %1 开始在充电桩 %2 开始充电 需要 %3 min")
                         .arg(topUserId).arg(i).arg(allUser[topUserId].NeedChargeTime)<< endl;
                // 详单
                Bill bill;
                bill.createBill(i, systime->hour(), systime->minute(), T_MODE);
                allBill.push_back(bill);
                allBill.end()->id = allBill.size()-1;   //设置详单编号
                CPtoBill[i] = allBill.end()->id;
                //报表
//                Report newreport("6-16");
//                CPToReport[i] = newreport;
                //用户

            }
        }
        else if (CP[i].state == CP_POWERING)    //充电桩处于充电状态
        {
            CP[i].OverPeriodUpdate();   //充电桩进行刷新
            allBill[CPtoBill[i]].updateBill(systime->hour());   //详单进行刷新
            allUser[CP[i].queue[0]].NeedChargeTime -= CPUPDATEPEIROD;
            if (CP[i].state == CP_FREE) //由充电状态转为空闲状态，说明充电结束
            {
                 allBill[CPtoBill[i]].finishBill(systime->hour(), systime->minute());
                 report[i].UpdateReport(CP[i], allBill[CPtoBill[i]]);   //得到所有的报表
            }
        }
    }
    FCallNum = haveCPFree;



}

CPServer::~CPServer()
{
    delete ui;
    timer->stop();
}

//处理事件
void CPServer::EventCome(QString ch, QString userId, QString mode, float degree)
{

    if (ch == "A")
    {
        userId.remove(0, 1);
        int uid = userId.toInt();

        if (degree == 0)   //中止充电
        {

            if (allUser[uid].prog == WAIT)  //在等候区中就直接进行修改
            {
                //取消充电
                qDebug() << "等候区取消充电" << endl;
                allUser[uid].mode = mode == "F" ? F_MODE : T_MODE;
                waitarea->ReGenerateNum(uid, allUser[uid].mode);
            }
            else if (allUser[uid].prog == CHARGEPOW || CHARGEWAIT)
            {
                qDebug() << "充电区取消充电" << endl;
                CP[allUser[uid].CPid].cancelPower(uid);
            }
        }
        else
        {
            if (waitarea->CurParkNum >= MAX_PARK_NUM)
            {
                qDebug()<< "当前等候区车位已满，用户无法进入充电桩" << endl;
            }


            //更改用户状态
            qDebug() << QString("处理用户 %1 的到来信息").arg(userId) << endl;
            allUser[uid].WaitNum = waitarea->CusArrive(uid, mode == "F" ? F_MODE : T_MODE);   //修改排队号

            qDebug() << "1======cpserver========" << endl;
            callnumToId[allUser[uid].WaitNum] = uid;
            qDebug() << QString("用户 %1 对应等待号 %2 检测 %3").arg(uid).arg(allUser[uid].WaitNum)
                        .arg(callnumToId[allUser[uid].WaitNum])
                     << endl;
            allUser[uid].mode = mode == "F" ? F_MODE : T_MODE;
            qDebug() << "3=======cpserver=======" << endl;
            allUser[uid].ChargeCapacity = degree;
            qDebug() << "4========cpserver=======" << endl;
            allUser[uid].prog = WAIT;
            float v = (allUser[uid].mode == F_MODE ? F_RATE : T_RATE);
            v /= 60;
            qDebug() << QString("充电速率 %1").arg(v) << endl;

            allUser[uid].NeedChargeTime = degree / v;
            qDebug() << QString("用户成功进入等候区， 需要时间 %1 min")
                        .arg(allUser[uid].NeedChargeTime) << endl;
        }
    }
    else if (ch == "B")
    {
        bool mode = (userId[0] == 'T') ? true : false;
        userId.remove(0, 1);
        int errID = mode ? userId.toInt() : userId.toInt() + MAX_F_CPNUM;

        if(degree == 0) //故障
        {
            //启动优先级调度
            waitarea->StartPriority = 1;

            //prioritySchedule(errID, mode);
            // 将故障充电桩的用户全部放入故障队列
            CP[errID].state = CP_ERROR; //标记为故障，之后任何调度都不找它
            waitarea->StartPriorityCallNum(mode, CP[errID].queue);
        }
        else //恢复
        {
            waitarea->StartPriority = 0;
            CP[errID].state = CP_FREE;
            /*
             * TODO
             * 若其它同类型充电桩中尚有车辆排队，则暂停等候区叫号服务
             * 将其它同类型充电桩中尚未充电的车辆合为一组，按照排队号码先后顺序重新调度
             * 调度完毕后，再重新开启等候区叫号服务。
             */
        }

    }

    else if (ch == "C")
    {
        // 修改请求
        userId.remove(0, 1);
        int uid = userId.toInt();

        if(allUser[uid].prog == WAIT) //允许在等候区修改
        {
            if (mode == allUser[uid].mode && degree != allUser[uid].ChargeCapacity)
                // 充电模式不变，仅更改充电度数
            {
                if (allUser[uid].prog == WAIT)
                {
                    allUser[uid].ChargeCapacity = degree;
                    float v = (mode == F_MODE ? F_RATE : T_RATE);
                    allUser[uid].NeedChargeTime = degree / (v / 60.0);
                }
                else if (allUser[uid].prog == CHARGEPOW ||
                         allUser[uid].prog == CHARGEWAIT)
                {
                    CP[allUser[uid].CPid].cancelPower(uid);
                }

            }
            //修改请求充电量
            if(mode == 'O')
            {
                allUser[uid].ChargeCapacity = degree;
                allUser[uid].updateRequest(degree, allUser[uid].mode);
            }
            //修改充电模式
            else
            {
                if (mode == allUser[uid].mode)
                allUser[uid].mode = (mode == 'F') ? true : false;

            }
        }
        else //不允许在充电区修改
        {
            //取消充电
            delCus(allUser[uid].CPid, allUser[uid].mode, uid);

            //修改请求充电量
            if(mode == 'O')
            {
                allUser[uid].ChargeCapacity = degree;
                waitarea->CusArrive(uid, allUser[uid].mode);
            }
            //修改充电模式
            else
            {
                allUser[uid].mode = (mode == 'F') ? true : false;
                waitarea->CusArrive(uid, allUser[uid].mode);
            }
        }
    }
//    switch (ch) {
//        case 'A':
//        {

//            break;
//        }
//        case 'B':
//        {
//            if(degree == 0) //故障
//            {
//                //启动优先级调度
//                waitarea->StartPriority = 1;
//                bool mode = (userId[0] == 'T') ? true : false;
//                userId.remove(0, 1);
//                int errID = mode ? userId.toInt() : userId.toInt() + MAX_F_CPNUM;
//                prioritySchedule(errID, mode);
//            }
//            else //恢复
//            {
//                waitarea->StartPriority = 0;
//                /*
//                 * TODO
//                 * 若其它同类型充电桩中尚有车辆排队，则暂停等候区叫号服务
//                 * 将其它同类型充电桩中尚未充电的车辆合为一组，按照排队号码先后顺序重新调度
//                 * 调度完毕后，再重新开启等候区叫号服务。
//                 */
//            }

//            break;
//        }
//        case 'C':
//        {
//            // 修改请求
//            userId.remove(0, 1);
//            int uid = userId.toInt();

//            if(userList[uid]->prog == WAIT) //允许在等候区修改
//            {
//                //修改请求充电量
//                if(mode == 'O')
//                {
//                    userList[uid]->ChargeCapacity = degree;
//                    userList[uid]->updateRequest(degree, userList[uid]->mode);
//                }
//                //修改充电模式
//                else
//                {
//                    userList[uid]->mode = (mode == 'F') ? true : false;

//                }
//            }
//            else //不允许在充电区修改
//            {
//                //取消充电
//                delCus(userList[uid]->CPid, userList[uid]->mode, uid);

//                //修改请求充电量
//                if(mode == 'O')
//                {
//                    userList[uid]->ChargeCapacity = degree;
//                    waitarea->CusArrive(uid, userList[uid]->mode);
//                }
//                //修改充电模式
//                else
//                {
//                    userList[uid]->mode = (mode == 'F') ? true : false;
//                    waitarea->CusArrive(uid, userList[uid]->mode);
//                }
//            }
//            break;
//        }
//        default:
//        {
//            break;
//        }
//    }
}


void CPServer::addSecs()
{
    // 空着就行
    *systime = systime->addSecs(60); //每过去1分钟，进行状态的刷新
}


/*
 *
 * TODO
 * 启动/停止充电信号
 */
void CPServer::signal_startpower()
{

}

void CPServer::signal_endpower()
{

}



bool CPServer::getLoginResult(QString name, QString pswd)
{
    // 服务器拿到收到的用户名密码判断是否登录成功，返回1表示成功
    User* user = db->loadUser(name, pswd);
    if(user == nullptr)
        return false;
    curUser = user;
    userList.insert(user->id, user);
    return true;
}

void CPServer::getEvent()
{
    QFile file ("../test.txt");  //建立一个文件对象

    //QFile file ("../change.txt");
    qDebug() << "开始读取文件";
    if (file.open(QFile::ReadOnly | QFile::Text)) //以只读模式打开文件成功
    {
        QTextStream in(&file);
        //in.setCodec(QTextCodec::codecForName("UTF-8"));
        in.seek(0); //把位置设置为0，以便从头开始读取

        while (!in.atEnd())   //判断文本流是否到达了文件尾
        {
            QString str = in.readLine(); //读取一行，不包含换行符
            qDebug() << str << endl;
            if (str.isEmpty())
            {
                qDebug() << "Failed to read line, no information matched";
                continue;
            }
            else
            {
                QString timeStr = str.section(',', 0, 0);
                QString eventStr = str.section(',', 1, 4);
                TimetoEvent[timeStr] = eventStr;
            }

        }

    }
    else
    {
        qDebug() << "File Open Failed";
    }

    file.flush();   //清空文件的缓冲区
    file.close();   //关闭文件
}

bool CPServer::getRegResult(QString name, QString pswd)
{
    // 服务器拿到收到的用户名密码判断是否注册成功，返回1表示成功
    // 检查用户是否已注册
    if(db->loadUser(name, pswd) != nullptr)
        return false;

    // 注册
    User* user = new User();
    user->name = name;
    user->pswd = pswd;
    db->storeUser(user);

    return true;
}

int CPServer::getQueueNum(QString name)
{
    // 服务器拿收到的用户名去取对应的排队号并返回这个数
    User* user = db->loadUser(name);
    if(userList.count(user->id) == 0)
        return 0;
    return waitarea->CusArrive(user->id, user->mode);
}

int CPServer::getPreCarNum(QString name)
{
    // 服务器拿收到的用户名去取对应的前车等待数并返回这个数
    User* user = db->loadUser(name);
    if(userList.count(user->id) == 0)
        return 0;
    return user->mode ? user->WaitNum - waitarea->it_F
                      : user->WaitNum - waitarea->it_T;
}

QByteArray CPServer::getSheet(QString name, int seq)
{
    // name为用户名，seq为用户在客户端点击详单列表的序号，假设详单们安装详单编号升序排列
    // 按如下格式，把10条详单信息（先用QString保存）依次转换成QByteArray再用换行符间隔
    QString ans;
    QByteArray ans_str = ans.toUtf8();
    ans_str.append('\n');

//    ui->serverEdit->appendPlainText("get sheet: " + ans);
    return ans_str;
}

QByteArray CPServer::getAllSheet(QString name)
{
    // ans中先存入该用户具有的详单总数，再依次append各个详单的时间和类型
    // 获取所有该用户的充电详单时间及充电类型（按编号升序填充到ans中）
    // 发送按照一个条目配一个换行符，如详单时间1\n类型1\n时间2\n类型2\n......
    QString ans;
    QByteArray ans_str = ans.toUtf8();
    ans_str.append('\n');

//    ui->serverEdit->appendPlainText("get sheet: " + ans);
    return ans_str;
}


bool CPServer::TurnOnCP(int CPid, bool mode)
//打开编号为id的充电桩
{
    if(!mode) // 慢充
    {
        if(CPid < 1 || CPid > MAX_T_CPNUM)
            return false;
        if (CP[CPid].ChargeId) // 编号不为0，说明已经打开
        {
            qDebug() << "充电桩已经打开，无需再次打开" << endl;
            return false;
        }
        else    // 编号为0
        {
            CP[CPid].ChargeId = CPid;
            qDebug() << "充电桩成功启动" << endl;
            return true;
        }
    }
    else // 快充
    {
        if(CPid < 1 || CPid > MAX_F_CPNUM)
            return false;
        if (CP[CPid].ChargeId) // 编号不为0，说明已经打开
        {
            qDebug() << "充电桩已经打开，无需再次打开" << endl;
            return false;
        }
        else    // 编号为0
        {
            CP[CPid].ChargeId = CPid;
            qDebug() << "充电桩成功启动" << endl;
            return true;
        }
    }


}

//关闭编号为id的充电桩
bool CPServer::TurnOffCP(int CPid, bool mode)
{
    if(!mode)
    {
        if(CPid < 1 || CPid > MAX_T_CPNUM)
            return 0;
        if (CP[CPid].ChargeId == 0) //编号为0，说明已经关闭
        {
            qDebug() << "充电桩已经关闭，无需再次关闭" << endl;
            return false;
        }
        else    //编号为0
        {
            CP[CPid].ChargeId = 0;
            qDebug() << "充电桩关闭" << endl;
            return true;
        }
    }
    else
    {
        if(CPid < 1 || CPid > MAX_F_CPNUM)
            return false;
        if (CP[CPid].ChargeId == 0) //编号为0，说明已经关闭
        {
            qDebug() << "充电桩已经关闭，无需再次关闭" << endl;
            return false;
        }
        else    //编号为0
        {
            CP[CPid].ChargeId = 0;
            qDebug() << "充电桩关闭" << endl;
            return true;
        }
    }
}

bool CPServer::NewCusArrive(int chargeType, int chargeQuantity)
{
    if (waitarea->CurParkNum >= MAX_PARK_NUM)    //停车区已满
    {
        qDebug() << "等候区车位已满"<<endl;
        return false;   //申请失败
    }
    curUser->mode = chargeType;
    curUser->ChargeCapacity = chargeQuantity;
    curUser->WaitNum = waitarea->CusArrive
            (curUser->id, chargeType);  //等候区去进行处理，生成排队号码
    return true;
}

void CPServer::delCus(int CPid, bool mode, int userID)
{
    for(int i = 0; i < CP[CPid].queue.size(); i++)
    {
        if(CP[CPid].queue[i] == userID)
            CP[CPid].queue[i] = -1;
    }
}


void CPServer::GotoChargeArea(bool mode, int CPid, int userId)
{
        CP[CPid].queue.push_back(userId);
}


// 调度

// 系统调度
int CPServer::sysSchedule(bool mode)
{
    int selectCP = 0;   //选中的充电桩
    int minTime = 0x7fffffff;   //当前的最短等待时间
    //QVector<ChargePile> CP;  //所有的充电桩
    if(mode) // 快充
    {
        for(int i = 1; i <= MAX_F_CPNUM; i++)
        {
            if(CP[i].queue.size() >= MAX_CHARGE_QUEUE_LEN)
                continue; //排队队列已满
            if (CP[i].state == CP_ERROR)
                continue;
            int tmpWaitTime = 0;    //在充电桩i时的完成充电所需时长
            QVector<int> iQueue = CP[i].queue;
            //遍历整个排队队列，求出等待时长
            for (int j=0; j<iQueue.size(); j++)
            {
                tmpWaitTime += allUser[iQueue[j]].NeedChargeTime;
            }
            if (minTime > tmpWaitTime)
            {
                minTime = tmpWaitTime;
                selectCP = CP[i].ChargeId;
            }
        }
    }
    else    // 慢充
    {
        for(int i = MAX_F_CPNUM+1; i <= MAX_T_CPNUM + MAX_F_CPNUM; i++)
        {
            if(CP[i].queue.size() >= MAX_CHARGE_QUEUE_LEN)
                continue; //排队队列已满
            if (CP[i].state == CP_ERROR)    //充电桩故障
                continue;
            int tmpWaitTime = 0;    //在充电桩i时的完成充电所需时长
            QVector<int> iQueue = CP[i].queue;
            //遍历整个排队队列，求出等待时长
            for (int j=0; j<iQueue.size(); j++)
            {
                tmpWaitTime += allUser[iQueue[j]].NeedChargeTime;
            }
            if (minTime > tmpWaitTime)
            {
                minTime = tmpWaitTime;
                selectCP = CP[i].ChargeId;
            }
        }
    }
    return selectCP;
}


int compare(const void *a, const void *b)
{
    return (*(int*)a - *(int*)b);
}


// 时间顺序调度
int CPServer::timeSchedule(int errID, bool mode)
{
    waitarea->CallFlag = false;
    QVector<int> q;
    QMap<int, int> map;

    if(mode)
    {
        // 将其它同类型充电桩中尚未充电的车辆与故障候队列中车辆合为一组
        for(int i = 0; i < MAX_F_CPNUM; i++)
        {
            for(int j = 1; j < CP[i].queue.size(); j++)
            {
                int uid = CP[i].queue[j];
                map.insert(userList[uid]->WaitNum, uid);
            }
            CP[i].queue.clear();
        }
        waitarea->TimeOrderCallNum(mode, q);
    }
    else
    {
        for(int i = 0; i < MAX_T_CPNUM; i++)
        {
            for(int j = 1; j < CP[i].queue.size(); j++)
            {
                int uid = CP[i].queue[j];
                map.insert(userList[uid]->WaitNum, uid);
            }
            CP[i].queue.clear();
        }
    }

    for(auto it = map.begin(); it != map.end(); it++)
    {
        q.push_back(it.value());
    }

    return waitarea->TimeOrderCallNum(mode, q);
}


// 优先级调度
int CPServer::prioritySchedule(int errID, bool mode)
{
    waitarea->CallFlag = false;
    if(mode)
    {
        waitarea->StartPriorityCallNum(mode, CP[errID].queue);
        return waitarea->PriorityCallNum(mode);
    }

    else
    {
        waitarea->StartPriorityCallNum(mode, CP[errID].queue);
        return waitarea->PriorityCallNum(mode);
    }
}

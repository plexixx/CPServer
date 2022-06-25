#include "cpserver.h"
#include "ui_cpserver.h"
#include <QDebug>

QVector<User> aCustomer;

CPServer::CPServer(QWidget *parent)
    : QObject(parent)
    , ui(new Ui::MainWindow)
    , CP(MAX_F_CPNUM + MAX_T_CPNUM +2)
    , report(MAX_F_CPNUM + MAX_T_CPNUM +2)
{
    db = new DB();
    FCallNum = 1;
    TCallNum = 1;

    //系统时间更新
    //systimer = new SysTimer();
    timer = new QTimer; //创建定时器
    systime = new QTime(5,55); // 6:00:00，启动后进入到5点55，
    connect(timer, SIGNAL(timeout()), this, SLOT(addSecs()));
    //连接槽函数，将timer的timeout行为，连接到updateTime函数中
    connect(timer, SIGNAL(timeout()), this, SLOT(updateTimeDeal()));

    //开启所有的充电桩
    for (int i=1; i<=MAX_F_CPNUM; i++)
        CP[i].turnOn(i, F_MODE);
    for (int i=MAX_F_CPNUM+1; i<=MAX_T_CPNUM + MAX_F_CPNUM; i++)
        CP[i].turnOn(i, T_MODE);

    timer->start(MS_PER_MIN);


//    connect(this, SIGNAL(signal_startpower()), this, );
//    connect(this, SIGNAL(signal_endpower()), this, );



    tcp_server = new QTcpServer;
    tcp_server->listen(QHostAddress::LocalHost, 6666);
    qDebug()<<("start listen to port 6666 of local host!") << endl;
    connect(tcp_server, SIGNAL(newConnection()), this, SLOT(onNewConnection()), Qt::QueuedConnection);

    //管理员
    //curmanager = new manager();
    waitarea = new WaitArea();  //等候区
    curUser = new User();



}

void CPServer::updateTimeDeal()
{
    qDebug() << QString("%1:%2 进行状态更新").arg(systime->hour(), systime->minute()) << endl;
        //1、 先考虑充电桩的状态，以及详单和报表的更新
//        emit signal_startpower();
    bool haveCPFree = 0;    //有充电桩空闲
    for (int i=1; i<=MAX_F_CPNUM; i++)  //遍历所有充电桩
    {
        if (CP[i].state == CP_FREE)   //充电桩处于空闲状态
        {
             qDebug() << QString("充电桩 %1 处于空闲状态且没人排队").arg(i) << endl;
            if (CP[i].queue.size() == 0)  //该充电桩里没人排队
            {
                haveCPFree = 1;
            }
            else    //有人排队，则可以开始工作
            {
                qDebug() << QString("充电桩 %1 处于空闲状态但有人排队").arg(i) << endl;
                //充电桩队列里有人排队，则可以开始充电
                //充电桩
                int topUserId = CP[i].queue[0];
                CP[i].start(aCustomer[topUserId].CurPower);
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
                 allBill[CPtoBill[i]].finishBill(systime->hour(), systime->minute());
                 report[i].UpdateReport(CP[i], allBill[CPtoBill[i]]);   //得到所有的报表
            }
        }
    }
    for (int i=MAX_F_CPNUM; i<=MAX_F_CPNUM + MAX_T_CPNUM; i++)  //遍历所有充电桩
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
                qDebug() << QString("充电桩 %1 处于空闲状态但有人排队").arg(i) << endl;
                //充电桩队列里有人排队，则可以开始充电
                //充电桩
                int topUserId = CP[i].queue[0];
                CP[i].start(aCustomer[topUserId].CurPower);
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
            if (CP[i].state == CP_FREE) //由充电状态转为空闲状态，说明充电结束
            {
                 allBill[CPtoBill[i]].finishBill(systime->hour(), systime->minute());
                 report[i].UpdateReport(CP[i], allBill[CPtoBill[i]]);   //得到所有的报表
            }
        }
    }

//        emit signal_endpower();
        //2、再考虑 等候区的状态

        //3、再考虑叫号和调度进入充电区问题

        //4、再考虑进入等候区的处理问题
        QVector<int> q;
        //先看一下可不可以叫号
        int calledNum;  //被叫的号
        int CPid;   //调度确定的充电桩
        if (FCallNum == 1)
        {
            if (waitarea->StartPriority == 1)
            {
                calledNum = waitarea->PriorityCallNum(F_MODE);
            }

            else
                calledNum = waitarea->CallNum(F_MODE);
            CPid = sysSchedule(F_MODE);    //叫号后面就是调度
            GotoChargeArea(F_MODE, CPid, callnumToId[calledNum]);
        }
        if (TCallNum == 1)
        {
            if (waitarea->StartPriority == 1)
                calledNum = waitarea->PriorityCallNum(T_MODE);

            else
                calledNum = waitarea->CallNum(T_MODE);
            CPid = sysSchedule(T_MODE);
        }


}

CPServer::~CPServer()
{
    delete ui;
    timer->stop();
}

bool CPServer::isEmpty(bool mode)
{
    int start = mode ? 0 : MAX_F_CPNUM;
    int end = mode ? MAX_F_CPNUM : CP.size();
    for(int i = start; i < end; i++)
    {
        for(int j = 0; j < MAX_CHARGE_QUEUE_LEN; j++)
        {
            if(CP[i].queue[j] > 0)
                return false; //存在车辆进行排队
        }
    }
    return true;
}


//处理事件
void CPServer::EventCome(char ch, QString userId, char mode, float degree)
{
    User* curUser;
    switch (ch) {
        case 'A':
        {
            userId.remove(0, 1);
            int uid = userId.toInt();
            curUser = userList[uid];
            if (waitarea->CurParkNum >= MAX_PARK_NUM)
            {
                qDebug()<< "当前等候区车位已满，用户无法进入充电桩" << endl;
            }

            //更改用户状态
            curUser->WaitNum = waitarea->CusArrive(uid, mode);   //修改排队号
            callnumToId[curUser->WaitNum] = curUser->id;
            curUser->mode = mode == 'F' ? true : false;
            curUser->ChargeCapacity = degree;
            break;
        }
        case 'B':
        {
            if(degree == 0) //故障
            {
                //启动优先级调度
                waitarea->StartPriority = 1;
                bool mode1 = (userId[0] == 'T') ? true : false;
                userId.remove(0, 1);
                int errID = mode1 ? userId.toInt() : userId.toInt() + MAX_F_CPNUM;
                prioritySchedule(errID, mode1);
            }
            else //恢复
            {
                waitarea->StartPriority = 0;
                /*
                 * 若其它同类型充电桩中尚有车辆排队，则暂停等候区叫号服务
                 * 将其它同类型充电桩中尚未充电的车辆合为一组，按照排队号码先后顺序重新调度
                 * 调度完毕后，再重新开启等候区叫号服务。
                 */
                bool mode1 = mode == 'F' ? true : false;
                int errID = mode1 ? userId.toInt() : userId.toInt() + MAX_F_CPNUM;
                if(!isEmpty(mode1)) //其它同类型充电桩中尚有车辆排队
                {
                    timeSchedule(errID, mode1);
                }
                else
                {
                    waitarea->CallFlag = true;
                }
            }

            break;
        }
        case 'C':
        {
            // 修改请求
            userId.remove(0, 1);
            int uid = userId.toInt();

            if(userList[uid]->prog == WAIT) //允许在等候区修改
            {
                //修改请求充电量
                if(mode == 'O')
                {
                    userList[uid]->ChargeCapacity = degree;
                    userList[uid]->updateRequest(degree, userList[uid]->mode);
                }
                //修改充电模式
                else
                {
                    userList[uid]->mode = (mode == 'F') ? true : false;

                }
            }
            else //不允许在充电区修改
            {
                //取消充电
                delCus(userList[uid]->CPid, userList[uid]->mode, uid);

                //修改请求充电量
                if(mode == 'O')
                {
                    userList[uid]->ChargeCapacity = degree;
                    waitarea->CusArrive(uid, userList[uid]->mode);
                }
                //修改充电模式
                else
                {
                    userList[uid]->mode = (mode == 'F') ? true : false;
                    waitarea->CusArrive(uid, userList[uid]->mode);
                }
            }
            break;
        }
        default:
        {
            break;
        }
    }
}


void CPServer::addSecs()
{
    // 空着就行
    systime->addSecs(60); //每过去1分钟，进行状态的刷新
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
    if (mode == F_MODE)
    {
        CP[CPid].queue.push_back(userId);
    }
    else
    {
        CP[CPid].queue.push_back(userId);
    }
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
            int tmpWaitTime = 0;    //在充电桩i时的完成充电所需时长
            QVector<int> iQueue = CP[i].queue;
            //遍历整个排队队列，求出等待时长
            for (int j=0; j<iQueue.size(); j++)
            {
                tmpWaitTime += aCustomer[iQueue[j]].NeedChargeTime;
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
            int tmpWaitTime = 0;    //在充电桩i时的完成充电所需时长
            QVector<int> iQueue = CP[i].queue;
            //遍历整个排队队列，求出等待时长
            for (int j=0; j<iQueue.size(); j++)
            {
                tmpWaitTime += aCustomer[iQueue[j]].NeedChargeTime;
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

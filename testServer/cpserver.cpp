#include "cpserver.h"
#include "ui_cpserver.h"
#include <QDebug>


CPServer::CPServer(QWidget *parent)
    : QObject(parent)
    , ui(new Ui::MainWindow)
    , F_CP(MAX_F_CPNUM + 1)
    , T_CP(MAX_T_CPNUM + 1)
{
    db = new DB();
    FCallNum = 1;
    TCallNum = 1;

    //系统时间更新
    systimer = new SysTimer();
    timer = new QTimer; //创建定时器
    systime = new QTime(6, 0); // 6:00:00

    connect(timer, SIGNAL(timeout()), this, SLOT(addSecs()));
    //连接槽函数，将timer的timeout行为，连接到updateTime函数中
    connect(timer, SIGNAL(timeout()), this, SLOT(updateTimeDeal()));

    timer->start(MS_PER_MIN);




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
    QVector<int> q;
    //先看一下可不可以叫号
    int calledNum;  //被叫的号
    int CPid;   //调度确定的充电桩
    if (FCallNum == 1)
    {
        if (waitarea->StartPriority == 1)
<<<<<<< HEAD
            calledNum = waitarea->PriorityCallNum(F_MODE);
=======
            waitarea->PriorityCallNum(F_MODE, q);
>>>>>>> cc03a5e608e501fd3b67fe3d4d73e9b33e82eaac
        else
            calledNum = waitarea->CallNum(F_MODE);
        CPid = sysSchedule(F_MODE);    //叫号后面就是调度
        GotoChargArea(F_MODE, CPid, callnumToId[calledNum]);
    }
    if (TCallNum == 1)
    {
        if (waitarea->StartPriority == 1)
<<<<<<< HEAD
            calledNum = waitarea->PriorityCallNum(T_MODE);
=======
            waitarea->PriorityCallNum(T_MODE, q);
>>>>>>> cc03a5e608e501fd3b67fe3d4d73e9b33e82eaac
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

//处理事件
void CPServer::EventCome(char ch, int userId, int mode, float degree)
{
    User* curUser;
    switch (ch) {
        case 'A':
            curUser = userList[userId];
            if (waitarea->CurParkNum >= MAX_PARK_NUM)
            {
                qDebug()<< "当前等候区车位已满，用户无法进入充电桩" << endl;
            }

            //更改用户状态
            curUser->WaitNum = waitarea->CusArrive(userId, mode);   //修改排队号
            callnumToId[curUser->WaitNum] = curUser->id;
            curUser->mode = mode;
            curUser-> ChargeCapacity = degree;
            break;
        case 'B':
            waitarea->StartPriority = 1;
            break;
        case 'C':
            break;
        default:
            break;
    }
}


void CPServer::addSecs()
{
    // 空着就行
    systime->addSecs(60); //过去1分钟
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
        if (T_CP[CPid].ChargeId) // 编号不为0，说明已经打开
        {
            qDebug() << "充电桩已经打开，无需再次打开" << endl;
            return false;
        }
        else    // 编号为0
        {
            T_CP[CPid].ChargeId = CPid;
            qDebug() << "充电桩成功启动" << endl;
            return true;
        }
    }
    else // 快充
    {
        if(CPid < 1 || CPid > MAX_F_CPNUM)
            return false;
        if (F_CP[CPid].ChargeId) // 编号不为0，说明已经打开
        {
            qDebug() << "充电桩已经打开，无需再次打开" << endl;
            return false;
        }
        else    // 编号为0
        {
            F_CP[CPid].ChargeId = CPid;
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
        if (T_CP[CPid].ChargeId == 0) //编号为0，说明已经关闭
        {
            qDebug() << "充电桩已经关闭，无需再次关闭" << endl;
            return false;
        }
        else    //编号为0
        {
            T_CP[CPid].ChargeId = 0;
            qDebug() << "充电桩关闭" << endl;
            return true;
        }
    }
    else
    {
        if(CPid < 1 || CPid > MAX_F_CPNUM)
            return false;
        if (F_CP[CPid].ChargeId == 0) //编号为0，说明已经关闭
        {
            qDebug() << "充电桩已经关闭，无需再次关闭" << endl;
            return false;
        }
        else    //编号为0
        {
            F_CP[CPid].ChargeId = 0;
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

void CPServer::GotoChargArea(bool mode, int CPid, int userId)
{
    if (mode == F_MODE)
    {
        F_CP[CPid].queue.push_back(userId);
    }
    else
    {
        T_CP[CPid].queue.push_back(userId);
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
        for(int i = 0; i < MAX_F_CPNUM; i++)
        {
            if(F_CP[i].queue.size() >= MAX_CHARGE_QUEUE_LEN)
                continue; //排队队列已满
            int tmpWaitTime = 0;    //在充电桩i时的完成充电所需时长
            QVector<int> iQueue = F_CP[i].queue;
            //遍历整个排队队列，求出等待时长
            for (int j=0; j<iQueue.size(); j++)
            {
                tmpWaitTime += aCustomer[iQueue[j]].NeedChargeTime;
            }
            if (minTime > tmpWaitTime)
            {
                minTime = tmpWaitTime;
                selectCP = F_CP[i].ChargeId;
            }
        }
    }
    else    // 慢充
    {
        for(int i = 0; i < MAX_T_CPNUM; i++)
        {
            if(F_CP[i].queue.size() >= MAX_CHARGE_QUEUE_LEN)
                continue; //排队队列已满
            int tmpWaitTime = 0;    //在充电桩i时的完成充电所需时长
            QVector<int> iQueue = T_CP[i].queue;
            //遍历整个排队队列，求出等待时长
            for (int j=0; j<iQueue.size(); j++)
            {
                tmpWaitTime += aCustomer[iQueue[j]].NeedChargeTime;
            }
            if (minTime > tmpWaitTime)
            {
                minTime = tmpWaitTime;
                selectCP = T_CP[i].ChargeId;
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
            for(int j = 1; j < F_CP[i].queue.size(); j++)
            {
                int uid = F_CP[i].queue[j];
                map.insert(userList[uid]->WaitNum, uid);
            }
            F_CP[i].queue.clear();
        }
        waitarea->TimeOrderCallNum(mode, q);
    }
    else
    {
        for(int i = 0; i < MAX_T_CPNUM; i++)
        {
            for(int j = 1; j < T_CP[i].queue.size(); j++)
            {
                int uid = T_CP[i].queue[j];
                map.insert(userList[uid]->WaitNum, uid);
            }
            T_CP[i].queue.clear();
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
        return waitarea->PriorityCallNum(mode, F_CP[errID].queue);
    else
        return waitarea->PriorityCallNum(mode, T_CP[errID].queue);
}

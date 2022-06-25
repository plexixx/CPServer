#include "tcpserver.h"
QMap <int, QString> UserList;
User curUsr;
bool loginState;
CPServer server;

TcpServer::TcpServer()
{
    run();
}
int TcpServer::run()
{
    qDebug()<<"[TcpServer]---------------------------------------------------start listen";
    if (this->listen(QHostAddress::LocalHost,6666)) // Any
    {
        qDebug()<<"[TcpServer]-------------------------------------------------listen sucess"<<endl;
    }
    else
    {
        qDebug()<<"[TcpServer]-------------------------------------------------listen failer"<<endl;
    }
    return 1;
}

void TcpServer::incomingConnection(qintptr socketDescriptor)
{
     qDebug()<<"[TcpServer]------------------------------------------new Connection !!! The Num:"<< tcpSocketConnetList.count() + 1<<endl;

     TcpSocket *tcpsocket = new TcpSocket();
     tcpsocket->setSocketDescriptor(socketDescriptor);
     tcpsocket->socketRun();
     connect(tcpsocket,SIGNAL(GetDataFromClient(QString ,int)),this,SLOT(Socket_Data_Processing(QString,int)));
     connect(tcpsocket,SIGNAL(ClientDisConnected(int)),this,SLOT(Socket_Disconnected(int)));
     connect(this,SIGNAL(WriteDataToClient(QString)),tcpsocket,SLOT(SendData_slot(QString)));
     tcpSocketConnetList.append(tcpsocket);
     //qDebug()<<"[TcpServer]-------------------------:"<<tcpSocketConnetList.count()<<endl;
     //qDebug()<<"[TcpServer]-------------------------:"<<tcpSocketConnetList.size()<<endl;
}


void TcpServer::Socket_Data_Processing(QString SendData,int descriptor)
{
    for(int i = 0; i < tcpSocketConnetList.count(); i++)
    {
        QTcpSocket *item = tcpSocketConnetList.at(i);
        if(item->socketDescriptor() == descriptor)        //通过descriptor在QList中查找对应的QTcpSocket对象
        {
            qDebug()<<"From ---> "<< item->peerAddress().toString() <<":"<<item->peerPort();
            qDebug()<<"[SendData]:"<< SendData ;
            qDebug()<<"End --- "<< endl;

            QList<QString> temp = SendData.split('\n');
            qDebug()<<"raw msg just split: "<<temp;
            QString name, pswd, ans, answer, answerType;
            QString requestType = temp.at(0).trimmed();
            bool res;
            int pileNum = MAX_F_CPNUM + MAX_T_CPNUM; // 充电桩总数目（慢充+快充）

            if(requestType == "clogin" || requestType == "mLogin")
            {
                name = temp.at(1).trimmed();
                pswd = temp.at(2).trimmed();
                curUsr.name = name;
                curUsr.pswd = pswd;
                curUsr.userType = 0;
                UserList[descriptor] = name;
                res = getLoginResult(name, pswd);
                if(requestType == "clogin")
                    answerType = "cLoginResult";
                else
                    answerType = "mLoginResult";

                if(res)
                    answer = "LoginSuccess";
                else
                    answer = "LoginFailure";
            }
            else if(requestType == "creg")
            {
                name = temp.at(1).trimmed();
                pswd = temp.at(2).trimmed();
                answerType = "RegResult";
                res = getRegResult(name, pswd);
                if(res)
                    answer = "RegSuccess";
                else
                    answer = "RegFailure";
            }
            else if (requestType == "cnum" || requestType == "cqueue")
            {
                int num;
                bool chargeType;
                if(requestType == "cqueue")
                {
                    answerType = "QueueResult";
                    num = getQueueNum(descriptor);
                }
                else
                {
                    answerType = "NumResult";
                    QString ctype = temp.at(1).trimmed();
                    QVariant onLineTemp = ctype;
                    chargeType = onLineTemp.toBool();
                    num = getPreCarNum(chargeType);
                }
                answer = QString::number(num, 10);
            }
            else if (requestType == "csheet")
            {
                answerType = "SheetResult";
                QString sseq = temp.at(1).trimmed();
                int seq = sseq.toInt();
                answer = getSheet(seq);
            }
            else if(requestType == "csheetlist")
            {
                answerType = "SheetListResult";
                answer = getAllSheet();
            }
            else if (requestType == "ccommit")
            {
                answerType = "CommitResult";
                QString ctype = temp.at(1).trimmed();
                QVariant onLineTemp = ctype;
                bool chargeType = onLineTemp.toBool();
                QString CQuantity = temp.at(2).trimmed();
                double chargeQuantity = CQuantity.toDouble();
                // 转存收到的充电类型和充电量
                curUsr.ChargeCapacity = chargeQuantity;
                curUsr.mode = chargeType;


                answer = "CommitSuccess";
                // 先只写了提交成功的情况，可以再根据数据库存入信息是否失败，加上提交失败的情况
            }
            else if (requestType == "mOpen")
            {
                QVariant tmp = temp.at(1).trimmed();
                bool type = tmp.toBool();
                QString isOpen = temp.at(2).trimmed();
                int No = temp.at(3).trimmed().toInt();
                res = getOpenResult(type, isOpen, No);
                answerType = "mOpenResult";
                if(type == true){
                    if(res)
                        answer = "OpenSuccess";
                    else
                        answer = "OpenFailure";
                }
                else
                {
                    if(res)
                        answer = "CloseSuccess";
                    else
                        answer = "CloseFailure";
                }
            }
            else if (requestType == "mStatus")
            {
                answerType = "mStatusResult";
                answer = QString::number(pileNum);

                QString str;

                for(int i = 1; i <= MAX_F_CPNUM + MAX_T_CPNUM; i++){
                    str += QString("%1,%2,%3,%4\n")
                            .arg(server.CP[i].state)
                            .arg(server.report[i].TotalChargeNum)
                            .arg(server.report[i].TotalChargeTime)
                            .arg(server.report[i].TotalChargeCapacity);
                    /*
                     * 将各充电桩的当前状态信息
                     * （是否正常工作、系统启动后累计充电次数、充电总时长、充电总电量）
                     * 组合成一条QString语句，赋值给str
                    */

                }
                answer = str;
            }
            else if(requestType == "mWait")
            {
                answerType = "mWaitResult";
                answer = QString::number(pileNum);

                QString str;

                for(int i = 1; i <= MAX_F_CPNUM + MAX_T_CPNUM; i++){
                    if(i <= MAX_F_CPNUM)
                        str += QString("F%1\n").arg(i);
                    else
                        str += QString("T%1\n").arg(i - MAX_F_CPNUM);
                    for(int j = 0; j < server.CP[i].queue.size(); j++)
                    {
                        int uid = server.CP[i].queue[j];
                        str += QString("%1,%2,%3,%4\n")
                                .arg(uid)
                                .arg(server.allUser[uid].MaxCapacity)
                                .arg(server.allUser[uid].ChargeCapacity)
                                .arg(server.allUser[uid].QueueTime);
                    }
                    /*
                     * 将各充电桩等候服务的车辆信息
                     * （用户ID、车辆电池总容量(度)、请求充电量(度)、排队时长）
                     * 组合成一条QString语句，赋值给str
                    */

                }
                answer = str;
            }
            else if(requestType == "mWaitArea")
            {
                answerType = "mWaitAreaResult";
                answer = getWaitArea();
            }
            else if (requestType == "mReport")
            {
                answerType = "mReportResult";
                answer = QString::number(pileNum);

                QString str;

                for(int i = 1; i <= MAX_F_CPNUM + MAX_T_CPNUM; i++){
                    /*
                     * 将报表的各项信息：
                     * 时间(日、周、月)、充电桩编号、累计充电次数、累计充电时长、
                     * 累计充电量、累计充电费用、累计服务费用、累计总费用
                     * 组合成一条QString语句，赋值给str
                    */
                    int pileID = (i <= MAX_F_CPNUM) ? i : i - MAX_F_CPNUM;
                    if(i <= MAX_F_CPNUM)
                        str += QString("F%1\n").arg(i);
                    else
                        str += QString("T%1\n").arg(i - MAX_F_CPNUM);

                    str += QString("2022-6-25,%1")
                            .arg(pileID)
                            .arg(server.report[i].TotalChargeNum)
                            .arg(server.report[i].TotalChargeTime)
                            .arg(server.report[i].TotalChargeCapacity)
                            .arg(server.report[i].TotalChargeFare)
                            .arg(server.report[i].TotalServeFare)
                            .arg(server.report[i].TotalFare);
                }
                answer = str;
            }

            // write response
            ans = answerType + "\n" + answer + "\n";
            emit WriteDataToClient(ans);
            break;
        }
    }
}

void TcpServer::Socket_Disconnected(int descriptor)
{
    for(int i = 0; i < tcpSocketConnetList.count(); i++)
    {
        QTcpSocket *item = tcpSocketConnetList.at(i);
        int temp = item->socketDescriptor();
        if(-1 == temp||temp == descriptor)            //测试中发现，当disconnected()信号发出，item->socketDescriptor()返回值已经为-1了，不能通过item->socketDescriptor() == descriptor来进行判断了。。。所以删除返回值为-1的QTcpSocket对象
        {
            tcpSocketConnetList.removeAt(i);//如果有客户端断开连接， 就将列表中的套接字删除
            item->deleteLater();
            qDebug()<< "[TcpSocket]---------------------------------Disconnect:" << descriptor << endl;
            return;
        }
    }
    return;
}

bool TcpServer::getLoginResult(QString name, QString pswd)
{
    // 服务器拿到收到的用户名密码判断是否登录成功，返回1表示成功
//    User* user = server.db->loadUser(name, pswd);
//    if(user == nullptr)
//        return false;
//    curUsr = *user;
//    return true;
    return server.getLoginResult(name, pswd);
}

bool TcpServer::getRegResult(QString name, QString pswd)
{
//    // 检查用户是否已注册
//    if(server.db->loadUser(name, pswd) != nullptr)
//        return false;

//    // 注册
//    User* user = new User();
//    user->name = name;
//    user->pswd = pswd;
//    server.db->storeUser(user);

//    return true;
    return server.getRegResult(name, pswd);
}

int TcpServer::getQueueNum(int descriptor)
{
    // 服务器拿收到的用户名去取对应的排队号并返回这个数
    // UserList[descriptor]可以直接获取用户名
    return server.getQueueNum(UserList[descriptor]);
}

int TcpServer::getPreCarNum(int descriptor)
{
    // 服务器拿收到的用户名去取对应的前车等待数并返回这个数
    return server.getPreCarNum(UserList[descriptor]);
}

QString TcpServer::getSheet(int seq)
{
    // name为用户名，seq为用户在客户端点击详单列表的序号，假设详单们安装详单编号升序排列
    // 按如下格式，把10条详单信息（先用QString保存）依次转换成QByteArray再用换行符间隔
    QString ans;

    qDebug()<<"get sheet: " << ans;
    return ans;
}

QString TcpServer::getAllSheet()
{
    // ans中先存入该用户具有的详单总数，再依次append各个详单的时间和类型
    // 获取所有该用户的充电详单时间及充电类型（按编号升序填充到ans中）
    // 发送按照一个条目配一个换行符，如详单时间1\n类型1\n时间2\n类型2\n......
    QString ans;

    qDebug()<<"get sheetlist: " << ans;
    return ans;
}

bool TcpServer::getOpenResult(bool type, QString isOpen, int No)
{
    // TODO: 服务器拿到数据开启/关闭充电桩。
    //type: false慢充，true快充
    //isOpen: 两种值"open"或者"close"
    //No：充电桩对应的编号
    //判断是否有对应的编号的充电桩。如果有对应编号的充电桩，并且开启/关闭成功，返回true。
    if(isOpen == "open")
    {
        return server.TurnOnCP(No, type);
    }
    else
    {
        return server.TurnOffCP(No, type);
    }

}

QString TcpServer::getWaitArea()
{
    //TODO:返回等候区各车辆的三元组消息，QString形式
    return "";
}

#ifndef CPSERVER_H
#define CPSERVER_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QApplication>
#include <QSet>
#include "systimer.h"
#include "manager.h"
#include <QTimer>
#include "chargepile.h"
#include "db.h"
#include <MacroDefine.h>
#include <waitarea.h>
#include <user.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

inline QVector<User> aCustomer;

//负责整个系统的运行
class CPServer : public QObject
{
    Q_OBJECT
private:
    SysTimer* systimer;         //系统时间
    QTimer * timer;             //计时器，用于系统时间的更新
    manager* curmanager;        //当前的管理员，本程序中只实现一个管理员
    QVector<ChargePile> F_CP;    //快充电桩
    QVector<ChargePile> T_CP;    //慢充电桩
    WaitArea* waitarea;
    User* curUser;


public:

    int sysSchedule(bool mode);  //系统调度
    int timeSchedule(int errID, bool mode); //时间顺序调度
    int prioritySchedule(int errID, bool mode); //优先级调度


    bool TurnOnCP(int CPid, bool mode);     //打开编号为id的充电桩
    bool TurnOffCP(int CPid, bool mode);    //关闭编号为id的充电桩
    bool NewCusArrive(int chargeType, int chargeQuantity);  //新用户要进入等候区

    CPServer(QWidget *parent = nullptr) : F_CP(MAX_F_CPNUM + 1), T_CP(MAX_T_CPNUM + 1) {};
    ~CPServer();

    DB* db;                                 //数据库
    QMap<QString, User*> userList;          //已登录用户表

    bool getLoginResult(QString, QString);  // 获取登录状态
    bool getRegResult(QString, QString);  // 获取注册状态
    QByteArray getSheet(QString, int seq);  // 获取详单信息
    QByteArray getAllSheet(QString); // 获取全部详单及其主要信息
    int getQueueNum(QString); // 获取排队号
    int getPreCarNum(QString, int); // 获取前车等待数
    bool CusChangAsk();

    Ui::MainWindow *ui;
    QTcpSocket* tcp_socket;  // 服务器检测到连接后创建的用于和客服端通信的socket
    QTcpServer* tcp_server;  // 服务器

    void closeEvent(QCloseEvent* event);  // 关闭连接事件


public slots:
    void onNewConnection();  // 收到新连接
    void onSocketStateChange(QAbstractSocket::SocketState socketstate);  // socket状态改变
    void onClientConnected();  // 和客户端建立连接
    void onClientDisconnected();   // 和客户端断开连接
    void onSocketReadyRead();  // socket可读
};
#endif // CPSERVER_H

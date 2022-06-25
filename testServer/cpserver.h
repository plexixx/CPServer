#ifndef CPSERVER_H
#define CPSERVER_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QApplication>
#include <QtAlgorithms>
#include <QSet>
//#include "systimer.h"
#include <QTimer>
#include <QTime>
#include "chargepile.h"
#include "db.h"
#include <MacroDefine.h>
#include <waitarea.h>
#include <user.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


//负责整个系统的运行
class CPServer : public QObject
{
    Q_OBJECT
private:
    //SysTimer* systimer;         //系统时间
    QTimer * timer;             //计时器，用于系统时间的更新
    User* curmanager;        //当前的管理员，本程序中只实现一个管理员
//    QVector<ChargePile> F_CP;    //快充电桩
//    QVector<ChargePile> T_CP;    //慢充电桩
    QVector<ChargePile> CP;
    QVector<Report> report;      //每个充电桩有一个报表
    WaitArea* waitarea;
    User* curUser;
    QVector<Bill> allBill;      //
    QMap<int, int>CPtoBill; //充电桩到详单的映射
    //QMap<int, Report>CPToReport;    //充电桩到报表的映射

    QTime* systime; // 系统时间

    bool FCallNum;
    bool TCallNum;



public:
    //处理事件
    void EventCome(char ch, QString userId, char mode, float degree);

    int sysSchedule(bool mode);  //系统调度
    int timeSchedule(int errID, bool mode); //时间顺序调度
    int prioritySchedule(int errID, bool mode); //优先级调度


    bool TurnOnCP(int CPid, bool mode);     //打开编号为id的充电桩
    bool TurnOffCP(int CPid, bool mode);    //关闭编号为id的充电桩
    bool NewCusArrive(int chargeType, int chargeQuantity);  //新用户要进入等候区

    CPServer(QWidget *parent = nullptr);
    ~CPServer();

    DB* db;                                 //数据库
    QMap<int, User*> userList;              //已登录用户表
    QMap<int, int> callnumToId;              //排队号码到用户id的映射

    bool getLoginResult(QString, QString);  // 获取登录状态
    bool getRegResult(QString, QString);  // 获取注册状态
    QByteArray getSheet(QString, int seq);  // 获取详单信息
    QByteArray getAllSheet(QString); // 获取全部详单及其主要信息
    int getQueueNum(QString); // 获取排队号
    int getPreCarNum(QString); // 获取前车等待数
    bool CusChangAsk();

    Ui::MainWindow *ui;
    QTcpSocket* tcp_socket;  // 服务器检测到连接后创建的用于和客服端通信的socket
    QTcpServer* tcp_server;  // 服务器

    void closeEvent(QCloseEvent* event);  // 关闭连接事件

    void GotoChargArea(bool mode, int CPid, int userId);


public slots:
    void updateTimeDeal();
    void addSecs();
    void signal_startpower();   //开始充电信号
    void signal_endpower();     //结束充电信号
};
#endif // CPSERVER_H

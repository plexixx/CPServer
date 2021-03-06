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


#include <QFile>
#include <QTextStream>
#include <QIODevice>
#include <QTextCodec>
#include <QByteArray>
#include <Windows.h>
//#include "qexcelexport.h"

#pragma execution_character_set("utf-8")

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


//负责整个系统的运行
class CPServer : public QObject
{
    Q_OBJECT
public:
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
    QVector<User> allUser;  //所有的的用户
    //QMap<int, Report>CPToReport;    //充电桩到报表的映射

    QTime* systime; // 系统时间

    //QExcelExport queueExcel;

    bool FCallNum;
    bool TCallNum;

    QMap<QString, QString> TimetoEvent; //时间对事件的映射

//    QFile *file_1;
//    QFile *file_2;
//    QFile *file_3;
//    QFile *file_4;
//    QFile *file_5;
//    QFile *file_6;

public:
    //处理事件
    void EventCome(QString ch, QString userId, QString mode, float degree);

    int sysSchedule(bool mode);  //系统调度
    int timeSchedule(int errID, bool mode); //时间顺序调度
    int prioritySchedule(int errID, bool mode); //优先级调度


    bool TurnOnCP(int CPid, bool mode);     //打开编号为id的充电桩
    bool TurnOffCP(int CPid, bool mode);    //关闭编号为id的充电桩
    bool NewCusArrive(int chargeType, int chargeQuantity);  //新用户要进入等候区
    void delCus(int CPid, bool mode, int userID); //从充电区删除用户
    void saveReport();

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
    bool CusChangeAsk();

    bool isEmpty(bool mode); // 检查同类型充电桩是否有车辆正在排队

    Ui::MainWindow *ui;
    QTcpSocket* tcp_socket;  // 服务器检测到连接后创建的用于和客服端通信的socket
    QTcpServer* tcp_server;  // 服务器

    void closeEvent(QCloseEvent* event);  // 关闭连接事件

    void GotoChargeArea(bool mode, int CPid, int userId);

    void getEvent();

    void testQueue();


public slots:
    void updateTimeDeal();
    void addSecs();
    void signal_startpower();   //开始充电信号
    void signal_endpower();     //结束充电信号
};
#endif // CPSERVER_H

#ifndef HEAD_H
#define HEAD_H

#include "bill.h"
#include "user.h"
#include "report.h"
#include "chargepile.h"
#include "MacroDefine.h"
#include "db.h"
#include "waitarea.h"
#include "cpserver.h"

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QList>
#include <QMap>
#include <QTime>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlRelation>
#include <QSqlError>

typedef struct userinfo
{
    QString name;
    QString pswd;
    bool type, userType; // 0--client 1--administrator
    double eleQuantity;
}UsrInfo;  // 用户信息结构体

//管理员只有一个，固定账号名和密码
static QString managerName = "12345";
static QString managerPswd = "12345";

// server
extern DB DB;
extern CPServer server;

// manager
extern SysTimer mySysTimer;
extern QVector<ChargePile> myChargePile;
extern QVector<Bill> myBill;
extern QVector<User> myUser;
extern QVector<Report> myReport;

// client
extern bool loginState;  // 登陆状态
extern User curUsr;
extern QMap <int, QString> UserList;


#endif // HEAD_H

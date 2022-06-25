#ifndef DB_H
#define DB_H

#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlRelation>
#include <QSqlError>
#include <MacroDefine.h>
#include <report.h>
#include <bill.h>
#include <user.h>
#include <QDebug>

static qlonglong ID = 1;

class DB
{
private:
    QSqlDatabase database;
    QSqlQuery query;

public:
    DB();

    void storeReport(Report* report); //存储报表
    void storeUser(User* user); //注册时存储用户信息
    void storeBill(Bill* sp);  //存储该详单信息

    void loadReport(Report *report); //执行对应的sql语句，载入报表
    User* loadUser(QString name, QString pswd); //搜索对应用户，不存在返回空指针
    User* loadUser(QString name); //搜索用户名对应信息
    QVector<Report> loadAllReport(); //执行对应的sql语句，载入报表
    QVector<User> loadAllUser();    //获取所有的用户信息
};

#endif // DB_H

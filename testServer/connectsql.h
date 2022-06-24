#ifndef CONNECTSQL_H
#define CONNECTSQL_H
#include "head.h"

static bool createConnection()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("sisDB.db");
    if (!db.open()) {
        QMessageBox::critical(0, "Cannot open the sisDB",
            "Unable to establish a sisDB connection.", QMessageBox::Cancel);
        return false;
    }
    QSqlQuery query(db);
    query.exec("create table if not exists SysTimer(year int, mon int, week int, "
               "day int, hour int, min int)");
    //QVector<int> queue:需要改成string类型存储，表中查找、添加时需要改变格式
    query.exec("create table if not exists ChargPile(ChargId int key, rate float, queue String, "
               "DayTotalNum int, DayTotalTime int, DayTotalPower float)");
    //SysTimer CreateTime/BeginPowerTime/EndPowerTime需要变成string类型存储
    query.exec("create table if not exists specification(id int key, CreateTime String, "
               "ChargId int, ChargCabacity float, ChargTime int, BeginPowerTime String, "
               "EndPowerTime String, ChargFare float, ServeFare float, TotalFare float)");
    query.exec("create table if not exists User(id int key, name String, pswd String)");
    query.exec("create table if not exists report(day int, week int, mon int, CPid int, "
               "TotalChargNum int, TotalChargTime int, TotalChargCabacity float, "
               "TotalChargFare float, TotalServeFare float, TotalFare float)");
    return true;
}

#endif // CONNECTSQL_H

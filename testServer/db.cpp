#include "db.h"

DB::DB()
{
    database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName("sisDB.db");

    if (!database.open()) {
        QMessageBox::critical(0, "Cannot open the sisDB",
            "Unable to establish a sisDB connection.", QMessageBox::Cancel);
    }

    query = QSqlQuery(database);

    //SysTimer CreateTime/BeginPowerTime/EndPowerTime需要变成string类型存储
    query.exec("create table if not exists bill(id int key, CreateTime String, ChargeId int,"
               "ChargeCabacity float, ChargeTime int, BeginPowerTime String, EndPowerTime String,"
               "ChargeFare float, ServeFare float, TotalFare float)");
    query.exec("create table if not exists user(id int key, name String, pswd String)");
    query.exec("create table if not exists report(day int, week int, mon int, CPid int, TotalChargeNum int, "
               "TotalChargeTime int, TotalChargeCabacity float, TotalChargeFare float, "
               "TotalServeFare float, TotalFare float)");
    query.exec("alter table report add constraint pk_name primary key (day,mon, CPid)");
    //qDebug() << "************************************!!!!!!!!!" << endl;
}

void DB::storeUser(User *user)
{
    query.prepare("insert into user(id int key, name String, pswd String)");

    query.bindValue(0, ID++);
    query.bindValue(1, user->name);
    query.bindValue(2, user->pswd);

    query.exec();
}


void DB::storeReport(Report* report)
{
    //保证day,week,mon,CPid必须整张表唯一
    query.prepare("insert into report(day,week,mon,CPid,TotalChargNum,TotalChargTime,"
                  "TotalChargCabacity,TotalChargFare,TotalServeFare,TotalFare)");

        query.bindValue(0,report->day);
        query.bindValue(1,report->week);
        query.bindValue(2,report->mon);
        query.bindValue(3,report->CPid);
        query.bindValue(4,report->TotalChargeNum);
        query.bindValue(5,report->TotalChargeTime);
        query.bindValue(6,report->TotalChargeCapacity);
        query.bindValue(7,report->TotalChargeFare);
        query.bindValue(8,report->TotalServeFare);
        query.bindValue(9,report->TotalFare);
    query.exec();
}

void DB::storeBill(Bill *sp)
{
    query.prepare("insert into bill(id,CreateTime,ChargeId,ChargeCabacity,ChargeTime,"
                  "BeginPowerTime,EndPowerTime,ChargeFare,ServeFare,TotalFare)");
    query.bindValue(0,sp->id);
    query.bindValue(1,sp->timeToString(sp->createHour, sp->createMin));
    query.bindValue(2,sp->ChargeId);
    query.bindValue(3,sp->ChargeCapacity);
    query.bindValue(4,sp->ChargeTime);
//    query.bindValue(5,sp->BeginPowerTime.toString());
//    query.bindValue(6,sp->EndPowerTime.toString());
    query.bindValue(1,sp->timeToString(sp->beginHour, sp->beginMin));
    query.bindValue(1,sp->timeToString(sp->endHour, sp->endMin));
    query.bindValue(7,sp->ChargeFare);
    query.bindValue(8,sp->ServeFare);
    query.bindValue(9,sp->TotalFare);
    query.exec();
}


void DB::loadReport(Report* report)
{
    query.exec(QString("select * from report where mon = %1 and week = %2 and day = %3 and CPid = %4")
               .arg(report->mon).arg(report->week).arg(report->day).arg(report->CPid));
    report->TotalChargeNum = query.value(3).toInt();
    report->TotalChargeTime = query.value(4).toInt();
    report->TotalChargeCapacity = query.value(5).toFloat();
    report->TotalChargeFare = query.value(6).toFloat();
    report->TotalServeFare = query.value(7).toFloat();
    report->TotalFare = query.value(8).toFloat();
}

QVector<Report> DB::loadAllReport()
{
    QVector<Report> allReport;
    query.exec("select * from report");
    while(query.next()){
        Report tmp;
        tmp.week = query.value(0).toInt();
        tmp.mon = query.value(1).toInt();
        tmp.CPid = query.value(2).toInt();
        tmp.TotalChargeNum = query.value(3).toInt();
        tmp.TotalChargeTime = query.value(4).toInt();
        tmp.TotalChargeCapacity = query.value(5).toFloat();
        tmp.TotalChargeFare = query.value(6).toFloat();
        tmp.TotalServeFare = query.value(7).toFloat();
        tmp.TotalFare = query.value(8).toFloat();
        allReport.push_back(tmp);
    }
    return allReport;
}



QVector<User> DB::loadAllUser()
{
    query.exec("select * from user");
    QVector<User> allUser;
    while(query.next()){
        User tmp;
        tmp.id = query.value(0).toInt();
        tmp.name = query.value(1).toString();
        tmp.pswd = query.value(2).toString();
        allUser.push_back(tmp);
    }
    return allUser;
}


User* DB::loadUser(QString name, QString pswd)
{
    query.exec(QString("select * from user where name = %1 and pswd = %2")
               .arg(name).arg(pswd));
    if(query.next()) {
        User* user = new User();
        user->id = query.value(0).toInt();
        user->name = query.value(1).toString();
        user->pswd = query.value(2).toString();
        return user;
    }
    return nullptr;
}


User* DB::loadUser(QString name)
{
    query.exec(QString("select * from user where name = %1")
               .arg(name));
    if(query.next()) {
        User* user = new User();
        user->id = query.value(0).toInt();
        user->name = query.value(1).toString();
        user->pswd = query.value(2).toString();
        return user;
    }
    return 0;
}

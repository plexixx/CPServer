#include "connectsql.h"

SysTimer mySysTimer;
QVector<ChargePile> myChargePile;
QVector<Bill> myBill;
QVector<User> myUser;
QVector<Report> myreport;

//将SysTimer转换成QString
QString toString(SysTimer mySysTimer){
    QString str = mySysTimer.toString();
    return str;
}

//将QString转换成SysTimer
//SysTimer stringTo(QString str){
//    SysTimer tmp;
//    tmp.Stringto(str);
//    return tmp;
//}


void loadData(){
    QSqlDatabase db = QSqlDatabase::database("QSQLITE");
    QSqlQuery query(db);
    query.exec("select * from SysTimer");
    if(query.next()){
        mySysTimer.year = query.value(0).toInt();
        mySysTimer.mon = query.value(1).toInt();
        mySysTimer.week = query.value(2).toInt();
        mySysTimer.day = query.value(3).toInt();
        mySysTimer.hour = query.value(4).toInt();
        mySysTimer.min = query.value(5).toInt();
    }
    query.exec("select * from ChargPile");
    while(query.next()){
        ChargePile tmp;
        tmp.ChargeId = query.value(0).toInt();
        tmp.rate = query.value(1).toFloat();
        tmp.DayTotalNum = query.value(2).toInt();
        tmp.DayTotalTime = query.value(3).toInt();
        tmp.DayTotalPower = query.value(4).toFloat();
        myChargePile.insert(0,tmp);
    }
    query.exec("select * from specification");
    while(query.next()){
        Bill tmp;
        tmp.id = query.value(0).toInt();
        tmp.CreateTime = stringTo(query.value(1).toString());
        tmp.ChargeId = query.value(2).toInt();
        tmp.ChargeCapacity = query.value(3).toFloat();
        tmp.ChargeTime = query.value(4).toInt();
        tmp.BeginPowerTime = stringTo(query.value(5).toString());
        tmp.EndPowerTime = stringTo(query.value(6).toString());
        tmp.ChargeFare = query.value(7).toFloat();
        tmp.ServeFare = query.value(8).toFloat();
        tmp.TotalFare = query.value(9).toFloat();
        myBill.insert(0,tmp);
    }
    query.exec("select * from User");
    while(query.next()){
        User tmp;
        tmp.id = query.value(0).toInt();
        tmp.name = query.value(1).toString();
        tmp.pswd = query.value(2).toString();
        myUser.insert(0,tmp);
    }
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
    }
    qDebug()<<"loadData succeed true!";
}

void storeData(){
    QSqlDatabase db = QSqlDatabase::database("QSQLITE");
    QSqlQuery query(db);
    query.exec("delete from SysTimer");
    query.prepare("insert into SysTimer(year,mon,week,day,hour,min)");
    query.bindValue(0,mySysTimer.year);
    query.bindValue(1,mySysTimer.mon);
    query.bindValue(2,mySysTimer.week);
    query.bindValue(3,mySysTimer.day);
    query.bindValue(4,mySysTimer.hour);
    query.bindValue(5,mySysTimer.min);
    query.exec();

    query.exec("delete from ChargPile");
    query.prepare("insert into ChargPile(ChargId,rate,DayTotalNum,DayTotalTime,DayTotalPower)");
    for(int i=0;i<myChargePile.size();i++){
        query.bindValue(0,myChargePile[i].ChargeId);
        query.bindValue(1,myChargePile[i].rate);
        query.bindValue(2,myChargePile[i].DayTotalNum);
        query.bindValue(3,myChargePile[i].DayTotalTime);
        query.bindValue(4,myChargePile[i].DayTotalPower);
    }
    query.exec();

    query.exec("delete from bill");
    query.prepare("insert into bill(id,CreateTime,ChargeId,ChargeCapacity,ChargeTime,"
                  "BeginPowerTime,EndPowerTime,ChargeFare,ServeFare,TotalFare)");
    for(int i=0;i<myBill.size();i++){
        query.bindValue(0,myBill[i].id);
        query.bindValue(1,toString(myBill[i].CreateTime));
        query.bindValue(2,myBill[i].ChargeId);
        query.bindValue(3,myBill[i].ChargeCapacity);
        query.bindValue(4,myBill[i].ChargeTime);
        query.bindValue(5,toString(myBill[i].BeginPowerTime));
        query.bindValue(6,toString(myBill[i].EndPowerTime));
        query.bindValue(7,myBill[i].ChargeFare);
        query.bindValue(8,myBill[i].ServeFare);
        query.bindValue(9,myBill[i].TotalFare);
    }
    query.exec();

    query.exec("delete from User");
    query.prepare("insert into User(id,name,pswd)");
    for(int i=0;i<myUser.size();i++){
        query.bindValue(0,myUser[i].id);
        query.bindValue(1,myUser[i].name);
        query.bindValue(2,myUser[i].pswd);
    }
    query.exec();

    query.exec("delete from report");
    query.prepare("insert into report(day,week,mon,CPid,TotalChargeNum,TotalChargeTime,"
                  "TotalChargeCapacity,TotalChargeFare,TotalServeFare,TotalFare)");
    for(int i=0;i<myreport.size();i++) {
        query.bindValue(0,myreport[i].day);
        query.bindValue(1,myreport[i].week);
        query.bindValue(2,myreport[i].mon);
        query.bindValue(3,myreport[i].CPid);
        query.bindValue(4,myreport[i].TotalChargeNum);
        query.bindValue(5,myreport[i].TotalChargeTime);
        query.bindValue(6,myreport[i].TotalChargeCapacity);
        query.bindValue(7,myreport[i].TotalChargeFare);
        query.bindValue(8,myreport[i].TotalServeFare);
        query.bindValue(9,myreport[i].TotalFare);
    }
    query.exec();
    qDebug()<<"storeData succeed true!";
}

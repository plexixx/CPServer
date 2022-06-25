#include "bill.h"
#include <QDebug>

Bill::Bill()
{

}

QString Bill::timeToString(int hour, int min)
{
    return  QString::number(hour, 10) + ":" + QString::number(min, 10);
}

void Bill::createBill(int uid, int chargeID,int hour, int min, int mode)
{
    this->uid = uid;
    this->ChargeId = chargeID;
    this->beginHour = hour;
    this->beginMin = min;

    ChargeFare = 0;
    ServeFare = 0;
    if (mode == F_MODE)
        electricity = F_RATE / 60.0;
    else
        electricity = T_RATE / 60.0;
    qDebug() << QString("用户%1开始充电").arg(uid) << endl;

}

void Bill::finishBill(int hour, int min)
{
    this->createHour = hour;
    this->createMin = min;

    this->endHour = hour;
    this->endMin = min;

    TotalFare = ChargeFare + ServeFare;

    qDebug() << "---------------详单生成---------------" << endl
             << "UID: " << this->uid << endl
             << "充电桩编号: " << ChargeId << endl
             << "充电电量: " << ChargeCapacity << endl
             << "开始时间: " << this->beginHour << ":" << this->beginMin << endl
             << "结束时间: " << this->endHour << ":" << this->endMin << endl
             << "服务费: " << this->ServeFare << endl
             << "充电费用: " << this->ChargeFare << endl
             << "总费用: " << this->TotalFare << endl
             << "-------------------------------------" << endl;
}

void Bill::updateBill(int hour)
{
//    float sub = ChargeFare;
    if ((hour >= 10 && hour < 15)||(hour >= 18 && hour < 21))
        ChargeFare += PEAK_UNIT_PRICE * electricity;
    else if ((hour >= 7 && hour < 10) || (hour >= 15 && hour < 18)
             || (hour >= 21 && hour < 23))
        ChargeFare += USUAL_UNIT_PRICE * electricity;
    else
        ChargeFare += VALLEY_UNIT_PRICE * electricity;

//    sub = ChargeFare - sub;
//    if(this->uid == 6)
//    qDebug() << "--------------更新详单----------"<<endl
//             <<sub << endl
//             <<ChargeFare << " " << this->uid << endl;
    ChargeCapacity += electricity;
    ChargeTime += CPUPDATEPEIROD;
    ServeFare += SERVE_UNIT_PRICE * electricity;
    //TotalFare += ChargeFare + ServeFare;
}

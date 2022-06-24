#include "bill.h"

Bill::Bill()
{

}

QString Bill::timeToString(int hour, int min)
{
    return  QString::number(hour, 10) + ":" + QString::number(min, 10);
}

void Bill::createBill(int chargeID,int hour, int min)
{
    this->ChargeId = chargeID;
    this->beginHour = hour;
    this->beginMin = min;


}

void Bill::finishBill(int hour, int min)
{

    this->createHour = hour;
    this->createMin = min;

    this->endHour = hour;
    this->endMin = min;
}

void Bill::updateBill(int hour)
{
    if ((hour > 10 && hour <= 15)||(hour > 18 && hour <= 21))
        ChargeFare += PEAK_UNIT_PRICE * electricity;
    else if ((hour > 7 && hour <= 10) || (hour > 15 && hour <= 18)
             || (hour > 21 && hour <= 23))
        ChargeFare += USUAL_UNIT_PRICE * electricity;
    else
        ChargeFare += VALLEY_UNIT_PRICE * electricity;

    ChargeCapacity += electricity;
    ChargeTime += CPUPDATEPEIROD;
    ServeFare += SERVE_UNIT_PRICE * electricity;
    TotalFare += ChargeFare + ServeFare;
}

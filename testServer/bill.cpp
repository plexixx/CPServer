#include "bill.h"

Bill::Bill()
{

}

void Bill::createBill(int chargeID, SysTimer *const beginPowerTime)
{
    this->ChargeId = chargeID;
    this->BeginPowerTime = *beginPowerTime;
    CreateTime = *beginPowerTime;
}

void Bill::finishBill(SysTimer *createTime, SysTimer *endPowerTime)
{
    EndPowerTime = *endPowerTime;
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

#include "report.h"

//报表
Report::Report(QString timeStr)
{
//    mon = Mon;
//    week = Week;
//    day = Day;
//    CPid = Id;
    this->timeStr = timeStr;
}

Report::Report()
{

}

void Report::UpdateReport(int DayTotalNum, int DayTotalTime, float DayTotalPower, float ChargeFare, float ServeFare)
{
    TotalChargeNum = DayTotalNum;
    TotalChargeTime = DayTotalTime;
    TotalChargeCapacity = DayTotalPower;

    TotalChargeFare += ChargeFare;
    TotalServeFare += ServeFare;
    TotalFare += ChargeFare + ServeFare;
}


void Report::NewDayUpdateReport(int mon, int week, int day)
{
    mon = mon;
    week = week;
    day = day;
    TotalChargeNum = 0;
    TotalChargeTime = 0;
    TotalChargeCapacity = 0;
    TotalChargeFare = 0;
    TotalServeFare = 0;
    TotalFare = 0;
}


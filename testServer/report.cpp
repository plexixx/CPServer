#include "report.h"

//报表
Report::Report(int Mon, int Week, int Day, int Id)
{
    mon = Mon;
    week = Week;
    day = Day;
    CPid = Id;
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


void Report::NewDayUpdateReport(SysTimer systimer)
{
    mon = systimer.mon;
    week = systimer.week;
    day = systimer.day;
    TotalChargeNum = 0;
    TotalChargeTime = 0;
    TotalChargeCapacity = 0;
    TotalChargeFare = 0;
    TotalServeFare = 0;
    TotalFare = 0;
}


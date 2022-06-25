#include "report.h"

//报表
Report::Report(QString timeStr, ChargePile CP, Bill bill)
{
//    mon = Mon;
//    week = Week;
//    day = Day;
//    CPid = Id;
//    this->timeStr = timeStr;
//    TotalChargeNum = DayTotalNum;
//    TotalChargeTime = DayTotalTime;
//    TotalChargeCapacity = DayTotalPower;

//    TotalChargeFare += ChargeFare;
//    TotalServeFare += ServeFare;
//    TotalFare += ChargeFare + ServeFare;
}

Report::Report()
{
    TotalChargeNum = 0;
    TotalChargeTime = 0;
    TotalChargeCapacity = 0;

    TotalChargeFare = 0;
    TotalServeFare = 0;
    TotalFare = 0;
    this->timeStr = "6-25";
}

void Report::UpdateReport(ChargePile CP, Bill bill)
{


    TotalChargeNum ++;
    TotalChargeTime += CP.ChargTime;
    TotalChargeCapacity += CP.ChargDu;

    TotalChargeFare += bill.ChargeFare;
    TotalServeFare += bill.ServeFare;
    TotalFare += bill.TotalFare;
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


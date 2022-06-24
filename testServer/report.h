#ifndef REPORT_H
#define REPORT_H

#include <systimer.h>

//报表
class Report
{
public:
    int day;    //日
    int week;   //周
    int mon;    //月
    int CPid;   //充电桩编号
    int TotalChargeNum;  //累计充电次数
    int TotalChargeTime; //累计充电时长，以Min为单位
    float TotalChargeCapacity;  //累计充电量
    float TotalChargeFare;  //累计充电费用
    float TotalServeFare;  //累计服务费用
    float TotalFare;       //累计总费用
public:
    Report(int Mon, int Week, int Day, int Id);
    Report();
//    void GetFromDb(db& database);
    //充电桩每完成一次充电就更新对应的报表，这里的输入是通过充电桩和报表传过来
    void UpdateReport(int DayTotalNum, int DayTotalTime,
                float DayTotalPower, float ChargeFare,
                float ServeFare);
    void NewDayUpdateReport(SysTimer systimer);    //新的一天到来，报表需要更新

};

#endif // REPORT_H

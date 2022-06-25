#ifndef CHARGEPILE_H
#define CHARGEPILE_H

#include<QVector>

#include<MacroDefine.h>



#include <QWidget>
#include<QTimer>
//充电桩
class ChargePile /*: public QObject*//*: public QWidget*/
{
    //Q_OBJECT
public:
    //关闭充电桩就是0，打开充电桩就是非0
    int ChargeId;    //充电桩id
    float rate;    //充电功率
    QVector<int> queue; //该充电桩的排队队列，为用户id的编号，正在充电的也算
    bool type;      //快慢类型
    int CurTotalNum;    //本次启动累计的充电次数
    int CurTotalTime;   //本次启动累计的充电总时长,分钟为单位
    float CurTotalPower;  //本次启动累计的总充电量，度为单位
    //今日数据需要先把本地存储的找到，并进行初始化
    //之后结束一天，就将其写入存储并重置为0
    int DayTotalNum;    //今日累计的充电次数
    int DayTotalTime;   //今日累计的充电总时长
    float DayTotalPower;  //今日累计的总充电量

    int ChargTime;      //本次充电的充电时间
    float ChargDu;      //本次充电的充电度数

    int state;      //充电桩状态

    //QTimer timer;   //当充电时就开启计时
    int SurplusPowtime; //剩余充电时长


public:
    ChargePile();
    ChargePile(int type);
    void turnOn(int id, int type);
    void turnOff(int id, int type);
    //开始充电，传入充电所需时间
    void start(int needTime);
    //每隔一段时间进行刷新
    void OverPeriodUpdate();
    //生成报表，即完成一次充电后立即更新报表
    //void CreateReport(SysTimer systimer, specification sp);
    void cancelPower(int id); //取消充电
    void loadTodayInfo(int TotalChargeNum, int TotalChargeTime,
                       float TotalChargeCapacity);  //从今日报表中加载今日信息
    //获取充电桩相关信息
    int getState()
    {
        return state;
    }


};

#endif // CHARGEPILE_H

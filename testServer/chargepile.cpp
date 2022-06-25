#include "chargepile.h"

ChargePile::ChargePile()
{
    ChargeId = 0;            //为0表示还没有开启
    state = CP_FREE;
    //connect()
}


ChargePile::ChargePile(int type)
{
    ChargeId = 0; //为0表示还没有开启
    this->type = type;
}

void ChargePile::turnOn(int id, int type)
{
    ChargeId = id;
    if (type == F_CHARGEPILE)
    {
        this->type = F_CHARGEPILE;
        this->rate = F_RATE;
    }
    else
    {
        this->type = T_CHARGEPILE;
        this->rate = T_RATE;
    }
    state = CP_FREE;
    CurTotalNum = 0;
    CurTotalTime = 0;
    CurTotalPower = 0;
    //loadTodayInfo(report);
}

void ChargePile::turnOff(int id, int type)
{
    ChargeId = 0;
    state = CP_OFF;
}

 void ChargePile::start(int needTime)
 {
    SurplusPowtime = needTime;
    if (SurplusPowtime <= 0)    //根本不需要时间，则不需要处理
        return ;
    CurTotalNum++;  //从启动开始，累计充电次数+1
    DayTotalNum++;  //今天的累计充电次数+1
    state = CP_POWERING;
 }

 void ChargePile::OverPeriodUpdate()
 {
     //过去了一段时间
    SurplusPowtime -= CPUPDATEPEIROD;
    CurTotalTime += CPUPDATEPEIROD;
    CurTotalPower += CPUPDATEPEIROD * rate;
    DayTotalTime += CPUPDATEPEIROD;
    DayTotalPower += CPUPDATEPEIROD * rate;

    if(SurplusPowtime <= 0) //充电完成
    {
        //timer.stop();   //停止计时
        SurplusPowtime = 0;
        state = CP_FREE;    //结束充电状态

    }
 }

void ChargePile::cancelPower(int id)
{
    if (id != queue[0] || state != CP_POWERING) //该用户还没有开始充电
    {
        for (int i=0; i<queue.size(); i++)
        {
            if (queue[i] == id)
            {
                 queue.erase(queue.begin()+i);
                 break;
            }
        }
    }
    else    //该用户正在充电
    {
//        if (timer.isActive())
//            timer.stop();
        SurplusPowtime = 0;
        state = CP_FREE;    //结束充电状态
        //emit signal_endpower();
    }
}

void ChargePile::loadTodayInfo(int TotalChargeNum, int TotalChargeTime,
                              float TotalChargeCapacity)
{
    DayTotalNum = TotalChargeNum;
    CurTotalTime = TotalChargeTime;
    CurTotalPower = TotalChargeCapacity;
}

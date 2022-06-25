#ifndef WAITAREA_H
#define WAITAREA_H

#include <QVector>
#include <stdio.h>
#include <QQueue>

#include <MacroDefine.h>
#include <algorithm>
#include <QWidget>
#include <chargepile.h>
#include <user.h>

extern QVector<User> aCustomer;

//等候区
class WaitArea
{

public:
    int CurParkNum;     //当前车位容量


    //排队队列所在的下标位置就是排队号
    //以下的下标指示器，均指的是实际存在的数据，第一个，如果等于队列的size，则说明队列为空
    QVector<int>FQueue; //快充模式的排队队列，只会增多不会减少
    int it_F;   //指向快充队列当前处理完成的位置，指向的确定位置是还没有处理的
    QVector<int>TQueue; //慢充模式的排队队列
    int it_T;   //指向慢充队列当前处理完成的位置

    QVector<int>E_FQueue; //故障的快充排队队列
    int it_EF;  //指向故障快充队列当前处理完成的位置
    QVector<int>E_TQueue; //故障的慢充排队队列
    int it_ET; //指向故障满充队列当前处理完成的位置

    QVector<int> MergedQueue; //时间顺序调度的一个新的队列
    int it_time;

    bool CallFlag;          //为1表示开启叫号服务
    bool StartPriority;     //为1表示充电桩故障时使用优先级调度

    WaitArea();
    //顾客来到等候区，将顾客加入队列，并返回排队号码
    //最后的生成号码如需区别可以将当前号码与充电模式进行结合
    int CusArrive(int cusId, int askType);

    void delCus(int cusId, int askType);

    //以下为各种叫号方式
    //注意，必须是判断充电桩有空位时才进行叫号
    int CallNum(int askType);     //普通叫号，指的是等候区的叫号，传入请求类型
    int PriorityCallNum(bool mode);   //优先级叫号，输入故障充电桩类型
    void StartPriorityCallNum(bool mode, QVector<int> q);   //开启优先级叫号
    //时间顺序叫号,传入充电类型，故障充电桩与同类型的打开的充电桩
    int TimeOrderCallNum(bool mode, QVector<int> q);
    //开启时间顺序叫号，当故障恢复时同样进行这个处理
    //将在充电区没有充电的用户重新放进一个队列
    void startTimeSegCall(int errID, QVector<ChargePile>& q);
    //同时叫几个号，输入快充电桩的剩余车位数目和慢充电桩的剩余输入数目
    //把得到的多个号存入传引用里面
    void M_CallNum(int Fnum, int Tnum, QVector<int>&Fcus, QVector<int>&Tcus);
    //重新生成排队号，返回对应的排队号，输入用户id，原来和现在的请求模式
    void ReGenerateNum(int id, int oldType);
};



#endif // WAITAREA_H

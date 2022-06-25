#ifndef BILL_H
#define BILL_H

//#include <QWidget>
//#include <systimer.h>
#include <MacroDefine.h>
#include<QString>

class Bill
{
public:
    int id;                 //详单编号，系统分配
    int mode;
    float electricity;      //充电桩每次更新时的充电度数
    //SysTimer CreateTime;    //详单生成时间
    int createHour;
    int createMin;
    int ChargeId;            //充电桩编号
    float ChargeCapacity;   //充电电量
    int ChargeTime;           //充电时长，以分钟为单位
    int beginHour;    //启动时间
    int beginMin;
    //SysTimer EndPowerTime;      //停止时间
    int endHour;
    int endMin;
    float ChargeFare;       //充电费用
    float ServeFare;       //服务费用
    float TotalFare;       //总费用


public:
    Bill();

public:
    //收到充电桩的开始充电信号之后，就创造一个详单
    void createBill(int chargeID,int hour, int min); //创造详单
    //收到充电桩完成一段时间的充电信号之后，就更新详单，输入当前所属时刻
    void updateBill(int hour);
    //收到充电桩结束充电的信号后，完成一次操作，详单生成
    void finishBill(int hour, int min);
    // 时间转为QString
    QString timeToString(int hour, int min);
};

#endif // BILL_H

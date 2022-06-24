#ifndef USER_H
#define USER_H
#include <QWidget>
#include <QApplication>
#include<QString>
#include<MacroDefine.h>

//用户类
//因为管理员只有一个，所以，不需要将管理员的相关信息进行存储
//永久化存储只针对于用户，所以，id信息也只是对于用户而言
class User
{
public:
    QString name;   //账户
    QString pswd;   //密码
    //用户id
    int id;

    bool userType; //用户类型

    float TotalFare;   //充电总费用
    float PowerFare;   //充电费用
    float ServeFare;   //服务费用

    //充电请求
    float CurPower;        //当前电量
    float ChargeCapacity;   //请求充电量
    float MaxCapacity;     //车辆电池总容量(度)
    bool mode;          //充电模式

    int prog;         //充电进度，分为在等候区还是在充电区
    int CPid;           //进入充电区，被分配的充电桩编号

    int WaitNum;        //排队号码
    float QueueTime;   //排队时长
    int NeedChargeTime;  //需要的充电时长，在充电时就不断递减
public:
    User();

    //更新用户状态
    void toChargeArea(); //进入充电区

//    bool operator<(const Customer& a, const Customer& b)//声明
//    {
//        return a.WaitNum < b.WaitNum;
//    }

    bool operator < ( const User &b ) const
    {
         return WaitNum < b.WaitNum;
    }
};

#endif // USER_H

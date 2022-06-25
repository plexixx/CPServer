#ifndef MACRODEFINE_H
#define MACRODEFINE_H

#define MAX_CHARGE_QUEUE_LEN 3   //充电桩最大排队队列
// 充电桩类型
#define F_CHARGEPILE 1   //快充电桩
#define T_CHARGEPILE 0   //慢充电桩
// 充电桩速度
#define F_RATE 30/60      //快充速度，度/min
#define T_RATE 10/60        //慢充速度
//充电桩状态
#define CP_POWERING 0   //充电中
#define CP_FREE 1       //空闲
#define CP_OFF  2       //关闭
#define CP_ERROR 3      //错误
//充电桩最大数目
#define MAX_F_CPNUM 2   //最大快充电桩数目
#define MAX_T_CPNUM 3   //最大慢充电桩数目

#define MS_PER_MIN  1000  //1秒就是一分钟
#define CPUPDATEPEIROD 1    //1min

//充电模式
#define F_MODE 1    //快充
#define T_MODE 0    //慢充
//充电进度
#define WAIT 0      //等候区
#define CHARGEWAIT 1     //充电区等待
#define CHARGEPOW 2  //充电区充电

#define MAX_PARK_NUM 10    //等候区最大停车数目

//相关费用
#define SERVE_UNIT_PRICE 0.8    //元/度
#define PEAK_UNIT_PRICE 1
#define USUAL_UNIT_PRICE    0.7
#define VALLEY_UNIT_PRICE 0.4

#define MAX_USER_NUM 30
#endif // MACRODEFINE_H

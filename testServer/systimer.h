#ifndef SYSTIMER_H
#define SYSTIMER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QWidget>
#include <MacroDefine.h>

//系统时间处理,只需要存储一条，用于系统启动时的时间恢复
class SysTimer : public QWidget
{
    Q_OBJECT
public:
    int year;   //年
    int mon;    //月
    int week;   //周
    int day;    //日
    int hour;   //时
    int min;    //分

public :
    SysTimer();
    SysTimer operator=(const SysTimer& systimer)//声明
    {
        year = systimer.year;
        mon = systimer.mon;
        week = systimer.week;
        day = systimer.day;
        hour = systimer.hour;
        min = systimer.min;
    }
//    int sub(const SysTimer left, const SysTimer right)//声明
//    {
//        if ()
//    }
    QString toString(); //转换为字符串
    void Stringto(QString str); //转换为字符串
signals:
    void signal_newday();   //新的一天到来
public slots:
    void updateTime();


};

#endif // SYSTIMER_H

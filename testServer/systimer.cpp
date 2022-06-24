#include "systimer.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

#define YEAR 0
#define MON 1
#define WEEK 2
#define DAY 3
#define HOUR 4
#define MIN 5



SysTimer::SysTimer()
{
    //初始化时加载文件路径
    QFile file("../file/SysTimer.txt");
    //以读写的方式打开文件，成功返回true，失败返回false

    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    //使用QTextStream一行一行读取文件
    {
        QTextStream in(&file);
        QString line = in.readLine();      //整行读取
        line.simplified();
        qDebug() << "读取系统时间"<<line << endl;

        if (line.length() > 0)
        {
            qDebug() << "成功读取一行" << endl;

            year = line.section(' ', YEAR, YEAR).toInt();
            mon = line.section(' ', MON, MON).toInt();
            week = line.section(' ', WEEK, WEEK).toInt();
            day = line.section(' ', DAY, DAY).toInt();
            hour = line.section(' ', HOUR, HOUR).toInt();
            min = line.section(' ', MIN, MIN).toInt();
        }
        else
        {
            qDebug() << "File Open Failed";
        }
    }
    file.flush();   //清空文件的缓冲区
    file.close();   //关闭文件
}

void SysTimer::updateTime(){
    min++;
    if (min == 60){
        min = 0;
        hour++;
        if (hour == 24)
        {
            hour = 0;
            day++;
            week++;
            if (week == 8)
            {
                week = 1;
            }
            if (((mon==1 || mon == 3 || mon == 5 ||
                 mon == 7 || mon == 8 || mon == 10 ||
                 mon == 12) && day==31) ||
                 ((mon==4 || mon == 6 || mon == 9 ||
                                 mon == 11 ) && day==30)||
                (mon == 2 && day==28))
            {
                day = 1;
                mon++;
                if (mon == 13){
                    mon = 1;
                    year++;
                }
            }
            emit signal_newday();
        }
    }
    qDebug() << "当前时间为 " << year<<"年 " <<mon<<"月 周"<<week
             << " " << day << "日 " <<hour<<"时 "<<min << "分" << endl;
}

QString SysTimer::toString()
{
    QString tmpStr;
    //该参数从左到右的含义为要转换的数字，字符串最小长度，进制，位数不足时用0补充
    tmpStr += QString("%1%2%3%4%5%6%").arg(year,4,10,QLatin1Char('0'))
                                      .arg(mon, 2, 10, QLatin1Char('0'))
                                      .arg(week, 1, 10, QLatin1Char('0'))
            .arg(day, 2, 10, QLatin1Char('0'))
            .arg(hour, 2, 10, QLatin1Char('0'))
            .arg(min, 2, 10, QLatin1Char('0'));
    return tmpStr;
}

void SysTimer::Stringto(QString str)
{
    year = str.mid(0, 4).toInt();   //4位
    mon = str.mid(4, 2).toInt();    //2位
    week = str.mid(6, 1).toInt();   //1位
    day = str.mid(7, 2).toInt();
    hour = str.mid(9, 2).toInt();
    min = str.mid(11, 2).toInt();
}

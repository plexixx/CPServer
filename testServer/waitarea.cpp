#include "waitarea.h"
#include <QDebug>

WaitArea::WaitArea()
{
    CurParkNum = 0;
    CallFlag = true;
    it_F = 0;
    it_T = 0;
    StartPriority = 0;
}

int WaitArea::CusArrive(int cusId, int askType)
{
   qDebug() << QString("CusArrive %1 %2").arg(cusId).arg(askType) << endl;
   CurParkNum ++;
    if (askType == F_MODE)
    {
        qDebug() << QString("1==============") << endl;
        FQueue.push_back(cusId);
        qDebug() << QString("2==============") << endl;
        return  FQueue.size()-1;
    }
    else
    {
        qDebug() << QString("3==============") << endl;
        TQueue.push_back(cusId);
        qDebug() << QString("4==============") << endl;
        return TQueue.size()-1;
    }
}


int WaitArea::CallNum(int askType)
{

    if (askType == F_MODE && it_F < FQueue.size())
    {
        int num = FQueue[it_F++];    //取出第一个号
        while (num == -1 )
        {
            if (it_F >= FQueue.size())
                return -1;
            num = FQueue[it_F++];
        }
        CurParkNum--;   //叫号叫走了当然空出车位了
        return  num;            //返回被叫号的号码
    }
    else if (askType == T_MODE && it_T < TQueue.size())
    {
        int num = TQueue[it_T++];    //取出第一个号
        while (num == -1)
        {
            if (it_T >= TQueue.size())
                return -1;
            num = TQueue[it_T++];
        }
        CurParkNum--;   //叫号叫走了当然空出车位了
        return  num;            //返回被叫号的号码
    }
    return -1;
}

void WaitArea::StartPriorityCallNum(bool mode, QVector<int> q)
{
    if(mode)    //快充
    {
        if(E_FQueue.size() == 0)
        {
            E_FQueue = q;
            it_EF = 0;
        }


    }
    else
    {
        if(E_TQueue.size() == 0)
        {
            E_TQueue = q;
            it_ET = 0;
        }


    }

}
int WaitArea::PriorityCallNum(bool mode)
{
    if(mode)
    {
        //如果故障队列都调度完毕，则开启普通调度
        if(it_EF >= E_FQueue.size())
        {
            StartPriority = 0;
            //CallFlag = true;
            return CallNum(mode);
        }

        int num = E_FQueue[it_EF++];    //取出第一个号

        return num;
    }
    else
    {

        //如果故障队列都调度完毕，则开启普通调度
        if(it_ET >= E_TQueue.size())
        {
            StartPriority = 0;
            //CallFlag = true;
            return CallNum(mode);
        }

        int num = E_TQueue[it_ET++];    //取出第一个号

        return num;
    }

    //如果故障队列都调度完毕，则开启普通调度
    StartPriority = 0;
    return CallNum(mode);

//    if (mode && it_EF < E_FQueue.size()) // 快充
//    {
//        int num = E_FQueue[it_EF++];    //取出第一个号
//        return  num;            //返回被叫号的号码
//    }
//    else if (!mode && it_ET < E_TQueue.size())
//    {
//        int num = E_TQueue[it_ET++];    //取出第一个号
//        return  num;            //返回被叫号的号码
//    }

}

void WaitArea::startTimeSegCall(int errID, QVector<ChargePile>& q)
{
    //先将所有需要重新调度的用户加入队列
//    for(int i=0; i<errCP.queue.size(); i++)
//    {
//        CusQueue.push_back(aCustomer[errCP.queue[i]]);
//    }
//    for (int i=0; i<SameTypeCP.size(); i++)
//    {
//        for (int j=1; j<SameTypeCP[i].queue.size(); j++)
//        {
//            CusQueue.push_back(aCustomer[SameTypeCP[i].queue[j]]);
//        }

//    }
//    //按排队号码先后顺序
//    std::sort(CusQueue.begin(), CusQueue.end());
//    it_time = 0;
}


int WaitArea::TimeOrderCallNum(bool mode, QVector<int> q)
{
    if(MergedQueue.size() == 0)
    {
        MergedQueue = q;
        it_time = 0;
    }
    if(it_time >= MergedQueue.size())
    {
        MergedQueue.clear();
        CallFlag = true;
        return CallNum(mode);
    }
    return MergedQueue[it_time++];
}


void WaitArea::M_CallNum(int Fnum, int Tnum, QVector<int>&Fcus, QVector<int>&Tcus)
{
    int dst = it_F+Fnum;
    for(;it_F < dst && it_F <= FQueue.size()-1; it_F++)
    {
        Fcus.push_back(FQueue[it_F]);
    }

    dst = it_T+Tnum;
    for(;it_T < dst && it_T <= FQueue.size()-1; it_T++)
    {
        Tcus.push_back(TQueue[it_T]);
    }
}

void WaitArea::ReGenerateNum(int id, int oldType)
{
    qDebug() << "开始等候区的删除" << endl;
    CurParkNum--;
    //从原来队列中删除
    int it;
    if (oldType == F_MODE)
    {
        it = it_F;
        bool flag = 0;
        while (it <= FQueue.size()-1)
        {
            if (FQueue[it] == id)
            {
                qDebug() << "找到取消充电的用户了" << endl;
                flag = 1;
                 break;
            }
            it++;
        }
        if (flag) FQueue[it] = -1;
    }
    else {
        it = it_T;
        bool flag = 0;
        while (it <= TQueue.size()-1)

        {
            if (TQueue[it] == id)
            {
                qDebug() << "找到取消充电的用户了" << endl;
                flag = 1;
                 break;
            }

            it++;
        }
        if (flag) TQueue[it] = -1;
    }
    qDebug() << "完成等候区的删除" << endl;
}

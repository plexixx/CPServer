﻿#include "waitarea.h"

extern QVector<User> aCustomer;

WaitArea::WaitArea()
{
    CurParkNum = 0;
    CallFlag = true;
}

int WaitArea::CusArrive(int cusId, int askType)
{
    if (CurParkNum >= MAX_PARK_NUM)
        return 0;

    if (askType == F_MODE)
    {
        FQueue.push_back(cusId);
        return  FQueue.size();
    }
    else
    {
        TQueue.push_back(cusId);
        return TQueue.size();
    }
}

int WaitArea::CallNum(int askType)
{
    if (askType == F_MODE && it_F < FQueue.size())
    {
        int num = FQueue[it_F++];    //取出第一个号
        return  num;            //返回被叫号的号码
    }
    else if (askType == T_MODE && it_T < TQueue.size())
    {
        int num = TQueue[it_T++];    //取出第一个号
        return  num;            //返回被叫号的号码
    }
    return 0;
}

int WaitArea::PriorityCallNum(bool mode, QVector<int> q)
{
    if(mode)
    {
        if(E_FQueue.size() == 0)
        {
            E_FQueue = q;
            it_EF = 0;
        }

        //如果故障队列都调度完毕，则开启普通调度
        if(it_EF >= E_FQueue.size())
        {
            CallFlag = true;
            return CallNum(mode);
        }

        int num = E_FQueue[it_EF++];    //取出第一个号
        return num;
    }
    else
    {
        if(E_TQueue.size() == 0)
        {
            E_TQueue = q;
            it_ET = 0;
        }

        //如果故障队列都调度完毕，则开启普通调度
        if(it_ET >= E_TQueue.size())
        {
            CallFlag = true;
            return CallNum(mode);
        }

        int num = E_TQueue[it_ET++];    //取出第一个号
        return num;
    }

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
//    //如果故障队列都调度完毕，则开启普通调度
//    return CallNum(errType);
}

void WaitArea::startTimeSegCall(int errID, QVector<ChargePile>& q)
{
    //先将所有需要重新调度的用户加入队列
    for(int i = 0; i < q.size(); i++)
    {

    }
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
    //从原来队列中删除
    int it;
    if (oldType == F_MODE)
    {
        it = it_F;
        for (; it<=FQueue.size()-1; it++)
        {
            if (FQueue[it] == id)
                break;
        }
        FQueue.erase(FQueue.begin()+it);
    }
    else {
        it = it_T;
        for (; it<=TQueue.size()-1; it++)
        {
            if (TQueue[it] == id)
                break;
        }
        TQueue.erase(FQueue.begin()+it);
    }

}

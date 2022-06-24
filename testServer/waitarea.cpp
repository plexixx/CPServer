#include "waitarea.h"

extern QVector<Customer> aCustomer;

WaitArea::WaitArea()
{
    CurParkNum = 0;
}

int WaitArea::CusArrive(int cusId, int askType)
{
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

int WaitArea::PriorityCallNum(int errType)
{
    if (errType == F_MODE && it_EF < E_FQueue.size())
    {
        int num = E_FQueue[it_EF++];    //取出第一个号
        return  num;            //返回被叫号的号码
    }
    else if (errType == T_MODE && it_ET < E_TQueue.size())
    {
        int num = E_TQueue[it_ET++];    //取出第一个号
        return  num;            //返回被叫号的号码
    }
    //如果故障队列都调度完毕，则开启普通调度
    return CallNum(errType);
}

void WaitArea::startTimeSegCall(ChargePile errCP, QVector<ChargePile>SameTypeCP)
{
    //先将所有需要重新调度的用户加入队列
    for(int i=0; i<errCP.queue.size(); i++)
    {
        CusQueue.push_back(aCustomer[errCP.queue[i]]);
    }
    for (int i=0; i<SameTypeCP.size(); i++)
    {
        for (int j=1; j<SameTypeCP[i].queue.size(); j++)
        {
            CusQueue.push_back(aCustomer[SameTypeCP[i].queue[j]]);
        }

    }
    //按排队号码先后顺序
    std::sort(CusQueue.begin(), CusQueue.end());
    it_time = 0;
}


int WaitArea::TimeOrderCallNum(int askType)
{
    if (it_time == CusQueue.size())   //所有的故障队列和未充电队列处理完毕
        return CallNum(askType);
    return CusQueue[it_time++].id;
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

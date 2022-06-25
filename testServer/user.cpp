#include "user.h"

User::User()
{

}

User::User(int i)
{
    this->id = i;
}

void User::toChargeArea()
{
    prog = CHARGEWAIT;   //状态调整
    havePower = 0;
}

void User::updateRequest(float degree, bool mode)
{
    ChargeCapacity = degree;
    this->mode = mode;
    NeedChargeTime = mode ? degree / F_RATE
                          : degree / T_RATE;

}

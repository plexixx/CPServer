#include "user.h"

User::User()
{

}

void User::toChargeArea()
{
    prog = CHARGEWAIT;   //状态调整
}

void User::updateRequest(float degree, bool mode)
{
    ChargeCapacity = degree;
    this->mode = mode;
    NeedChargeTime = mode ? degree / F_RATE
                          : degree / T_RATE;

}

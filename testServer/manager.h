#ifndef MANAGER_H
#define MANAGER_H
#include<MacroDefine.h>
#include "user.h"

//管理员方面
class manager : public User
{
    //Q_OBJECT
private:



public:
    manager();

//signals:
//    void signal_TakeOnCP(int CPid);
//    void signal_TakeOFFCP(int CPid);
};

#endif // MANAGER_H

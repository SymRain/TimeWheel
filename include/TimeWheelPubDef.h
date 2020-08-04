#ifndef TIME_WHEEL_PUB_DEF_H
#define TIME_WHEEL_PUB_DEF_H
enum _eTimeWheelSetKind 
{
    _eTiWhSetKind_SetMinTime = 1
};

////////////////////////Set Function Arg///////////////////////////
struct _sTimeWheel_Time
{
    long l_Sec;
    long l_uSec;
};////////////_eTiWhSetKind_SetMinTime///////////////////////

#endif
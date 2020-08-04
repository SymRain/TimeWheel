#include "../include/TimeWheel_Api.h"
#include "../include/TimeWheel.h"

bool _cTimeWheelInterface::AddFunction(void *(*p_FuncAdd)(void *), void *p_Arg, unsigned long long unll_FrameBeforeRun, bool b_SetRepeat)
{
    return _cTimingWheelRunner::GetInterface().AddFunction(p_FuncAdd, p_Arg, unll_FrameBeforeRun, b_SetRepeat);
}

void _cTimeWheelInterface::Init(unsigned int un_SetFrame[], int n_SetWheelCount)
{
    _cTimingWheelRunner::GetInterface().Init(un_SetFrame, n_SetWheelCount);
}

void _cTimeWheelInterface::Start()
{
    _cTimingWheelRunner::GetInterface().Start();
}

void _cTimeWheelInterface::Stop()
{
    _cTimingWheelRunner::GetInterface().Stop();
}

bool _cTimeWheelInterface::Set(void *p_Set, _eTimeWheelSetKind e_Kind)
{
    return _cTimingWheelRunner::GetInterface().Set(p_Set, e_Kind);
}

void _cTimeWheelInterface::DeleteAllFunctionInOneFrame(int un_Frame)
{
    _cTimingWheelRunner::GetInterface().DeleteAllFunctionInOneFrame(un_Frame);
}
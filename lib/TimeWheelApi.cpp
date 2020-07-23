#include "../include/TimeWheel_Api.h"
#include "../include/TimeWheel.h"


bool _cTimeWheelInterface::AddFunction(void *(*p_FuncAdd)(void *), void*p_Arg, unsigned long long unll_FrameBeforeRun,bool b_SetRepeat)
{
    return pc_TimingWheelRunner->AddFunction(p_FuncAdd, p_Arg, unll_FrameBeforeRun,b_SetRepeat);
}

void _cTimeWheelInterface::Init(unsigned int un_SetFrame[], int n_SetWheelCount)
{
    pc_TimingWheelRunner->Init(un_SetFrame,n_SetWheelCount);
}

void _cTimeWheelInterface::Start()
{
    pc_TimingWheelRunner->Start();
}

void _cTimeWheelInterface::Stop()
{
    pc_TimingWheelRunner->Stop();
}

void _cTimeWheelInterface::Set(void *p_Set, int n_Kind)
{
    pc_TimingWheelRunner->Set(p_Set,n_Kind);
}

void _cTimeWheelInterface::DeleteAllFunctionInOneFrame(int un_Frame)
{
    pc_TimingWheelRunner->DeleteAllFunctionInOneFrame(un_Frame);
}
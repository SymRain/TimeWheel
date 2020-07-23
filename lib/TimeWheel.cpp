#include "../include/TimeWheel.h"
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>

#define NAMED_PIPE_NAME "./Temp.Pipe"

enum _eTimingWheelCommand
{
    _eiAddFunction = 1,
    _eiDeleteFunction=2
};

struct _sFunctionInsert
{
    _eTimingWheelCommand e_Type;
    _cUserFunction c_InsertFunction;
};

void *TimingWheelNotifyFunction(void *);
void SysTimeHandlePro(int);

void _cTimingWheel::AddFunction(_cUserFunction *ps_UserFunction)
{
    if (ps_UserFunction->un_WheelInsert == 256)
        ps_UserFunction->un_WheelInsert = un_FrameID;
    unsigned int un_FrameInsert = ps_UserFunction->unll_FrameBeforeRun % un_Frame;
    Arr_Frame[un_FrameInsert].un_UserFunctionCount++;
    ps_UserFunction->p_Next = Arr_Frame[un_FrameInsert].p_UserFunctionRoot;
    ps_UserFunction->p_Above = nullptr;
    Arr_Frame[un_FrameInsert].p_UserFunctionRoot = ps_UserFunction;
}

void _cTimingWheel::AddFunctionRepeat(_cUserFunction *ps_UserFunction)
{
    unsigned int un_FrameInsert = ps_UserFunction->unll_FrameBeforeRun % un_Frame;
    Arr_Frame[un_FrameInsert].un_UserFunctionCount++;
    ps_UserFunction->p_Next = Arr_Frame[un_FrameInsert].p_UserFunctionRepeatRoot;
    ps_UserFunction->p_Above = nullptr;
    Arr_Frame[un_FrameInsert].p_UserFunctionRepeatRoot = ps_UserFunction;
}

unsigned int _cTimingWheel::GetReadyFunction(_cUserFunction *&ps_UserFunction)
{
    ps_UserFunction = p_UserFunctionReadyToRun;
    return un_UserFunctionReadyToRunCount;
}

void _cTimingWheel::JumpToNextFrame()
{
    _cUserFunction *ps_UserFunctionGetAbove = nullptr;
    if (p_AboveWheel != nullptr)
    {
        unsigned int un_ReadyFunctionAboveCount = p_AboveWheel->GetReadyFunction(ps_UserFunctionGetAbove);
        _cUserFunction *ps_UserFunctionGetAboveSeek = ps_UserFunctionGetAbove;
        ps_UserFunctionGetAbove = ps_UserFunctionGetAbove->p_Next;
        while (ps_UserFunctionGetAbove != nullptr)
        {
            AddFunction(ps_UserFunctionGetAboveSeek);
            ps_UserFunctionGetAbove = ps_UserFunctionGetAbove->p_Next;
        }
        AddFunction(ps_UserFunctionGetAbove);
    }
    un_ActiveFrame %= un_Frame;

    p_UserFunctionReadyToRun = Arr_Frame[un_ActiveFrame].p_UserFunctionRoot;
    un_UserFunctionReadyToRunCount = Arr_Frame[un_ActiveFrame].un_UserFunctionCount;
    _cUserFunction *ps_UserFunctionSeek = Arr_Frame[un_ActiveFrame].p_UserFunctionRoot;
    Arr_Frame[un_ActiveFrame].p_UserFunctionRoot = nullptr;
    Arr_Frame[un_ActiveFrame].un_UserFunctionCount = 0;
    while (ps_UserFunctionSeek != nullptr)
    {
        if (ps_UserFunctionSeek->un_WheelInsert == un_FrameID && ps_UserFunctionSeek->b_IsRepeat == true && ps_UserFunctionSeek->b_IsActive==true)
        {
            _cUserFunction *ps_UserFunctionRepeatInsert = new _cUserFunction(*ps_UserFunctionSeek);
            AddFunction(ps_UserFunctionRepeatInsert);
        }
        ps_UserFunctionSeek = ps_UserFunctionSeek->p_Next;
    }
    ++un_ActiveFrame;
}
void _cTimingWheel::ClearOneFrameFunction(unsigned long long unll_SetFrame)
{
    _cUserFunction *ps_UserFunctionSeek = Arr_Frame[unll_SetFrame % un_Frame].p_UserFunctionRoot;
    while (ps_UserFunctionSeek != nullptr)
    {
        if (ps_UserFunctionSeek->unll_FrameBeforeRun == unll_SetFrame)
        {
            ps_UserFunctionSeek->b_IsActive = false;
        }
        ps_UserFunctionSeek = ps_UserFunctionSeek->p_Next;
    }
}
void _cTimingWheel::Init(unsigned int un_SetFrame, unsigned int un_SetFrameID)
{
    Arr_Frame = new _cTimingWheelFrame[un_SetFrame];
    p_UserFunctionRoot = nullptr;
    p_UserFunctionReadyToRun = nullptr;
    un_UserFunctionCount = 0;
    un_UserFunctionReadyToRunCount = 0;
    un_Frame = un_SetFrame;
    un_FrameID = un_SetFrameID;
}

bool _cTimingWheelRunner::AddFunction(void *(*p_FuncAdd)(void *), void *p_Arg, unsigned long long unll_FrameBeforeRun, bool b_SetRepeat)
{
    if (b_IsRun == false)
    {
        AddFunctionDirectly(p_FuncAdd, p_Arg, unll_FrameBeforeRun, b_SetRepeat);
    }
    else
    {
        _sFunctionInsert s_FunctionInsert;
        s_FunctionInsert.e_Type = _eiAddFunction;

        s_FunctionInsert.c_InsertFunction.b_IsRepeat = b_SetRepeat;
        s_FunctionInsert.c_InsertFunction.unll_FrameBeforeRun = unll_FrameBeforeRun;
        s_FunctionInsert.c_InsertFunction.p_UserFunction = p_FuncAdd;
        s_FunctionInsert.c_InsertFunction.p_UserFunctionArg = p_Arg;
        if (0 == write(n_PipeToUser[1], &s_FunctionInsert, sizeof(_sFunctionInsert)))
        {
            return false;
        }
    }
    return true;
}

void _cTimingWheelRunner::Init(unsigned int un_SetFrame[], int n_SetWheelCount)
{
    b_IsRun = false;
    if (n_SetWheelCount > 255)
        return;
    un_TimingWheelCount = (unsigned int)n_SetWheelCount;
    for (int n_Index = 0; n_Index < n_SetWheelCount; ++n_Index)
    {
        Arr_TimingWheel[n_Index].Init(un_SetFrame[n_Index], n_Index);
    }
    for (int n_Index = 0; n_Index < 5; ++n_Index)
    {
        Arr_UserFunctionWaitArr[n_Index] = nullptr;
    }
    un_FunctionWaitArrUsed = 0;
    un_FrameTime = 1000;
    e_ElimiAlg = _eTimingFirst;

    s_TimerSetArg.it_interval.tv_sec = 1;
    s_TimerSetArg.it_interval.tv_usec = 0;

    s_TimerSetArg.it_value.tv_sec = 1;
    s_TimerSetArg.it_value.tv_usec = 0;

    pipe(n_PipeToGetRes);
    pipe(n_PipeToNotify);
    mkfifo(NAMED_PIPE_NAME, S_IRUSR | S_IWUSR);
    n_PipeToUser[0] = open(NAMED_PIPE_NAME, O_RDONLY | O_NONBLOCK);
    n_PipeToUser[1] = open(NAMED_PIPE_NAME, O_WRONLY | O_NONBLOCK, S_IRWXU);

    signal(SIGALRM, SysTimeHandlePro);
    pthread_t pthr_ThreadID;
    int n_x = pthread_create(&pthr_ThreadID, NULL, TimingWheelNotifyFunction, NULL);
}
void _cTimingWheelRunner::Set(void *p_Set, int n_Kind)
{
}
_cTimingWheelRunner::_cTimingWheelRunner()
{
    un_TimingWheelCount = 0;
    b_IsRun = false;
}
_cTimingWheelRunner::~_cTimingWheelRunner()
{
}
void _cTimingWheelRunner::Start()
{
    setitimer(ITIMER_REAL, &s_TimerSetArg, NULL);
    b_IsRun = true;
}
void _cTimingWheelRunner::Stop()
{
    itimerval s_StopArg;
    s_StopArg.it_interval.tv_sec = 0;
    s_StopArg.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &s_StopArg, NULL);
    b_IsRun = false;
}

void _cTimingWheelRunner::AddFunctionDirectly(void *(*p_FuncAdd)(void *), void *p_Arg, unsigned long long unll_FrameBeforeRun, bool b_SetRepeat)
{
    _cUserFunction *ps_InsertUserFunction = new _cUserFunction;
    ps_InsertUserFunction->p_UserFunction = p_FuncAdd;
    ps_InsertUserFunction->unll_FrameBeforeRun = unll_FrameBeforeRun;
    ps_InsertUserFunction->b_IsRepeat = b_SetRepeat;
    ps_InsertUserFunction->p_UserFunctionArg = p_Arg;
    for (int n_Index = 0; n_Index < un_TimingWheelCount; ++n_Index)
    {
        unll_FrameBeforeRun /= (unsigned long long)Arr_TimingWheel[n_Index].GetFrameCount();
        if (unll_FrameBeforeRun == 0)
        {
            Arr_TimingWheel[n_Index].AddFunction(ps_InsertUserFunction);
            break;
        }
    }
}

void _cTimingWheelRunner::Run()
{
    _sFunctionInsert s_FunctionInsert;
    if (-1 != read(n_PipeToUser[0], &s_FunctionInsert, sizeof(_sFunctionInsert)))
    {
        switch (s_FunctionInsert.e_Type)
        {
        case _eiAddFunction:
            AddFunctionDirectly(s_FunctionInsert.c_InsertFunction.p_UserFunction, s_FunctionInsert.c_InsertFunction.p_UserFunctionArg, s_FunctionInsert.c_InsertFunction.unll_FrameBeforeRun, s_FunctionInsert.c_InsertFunction.b_IsRepeat);
            break;
        case _eiDeleteFunction:
            DeleteAllFunctionInOneFrameDirectly(s_FunctionInsert.c_InsertFunction.unll_FrameBeforeRun);
            break;
        default:
            break;
        }
    }

    for (int n_Index = 0; n_Index < un_TimingWheelCount; ++n_Index)
    {
        Arr_TimingWheel[n_Index].JumpToNextFrame();
    }

    if (0 != Arr_TimingWheel[0].GetReadyFunction(Arr_UserFunctionWaitArr[0]))
    {
        write(n_PipeToNotify[1], Arr_UserFunctionWaitArr[0], sizeof(_cUserFunction));
        Arr_UserFunctionWaitArr[0] = nullptr;
    }
    Arr_UserFunctionWaitArr[0] = nullptr;
}
_cUserFunction *_cTimingWheelRunner::GetWaitFunction(int &n_FunctionCount)
{
}

void _cTimingWheelRunner::DeleteAllFunctionInOneFrameDirectly(unsigned long long unll_SetFrame)
{
    for (int n_Index = 0; unll_SetFrame != 0; unll_SetFrame /= Arr_TimingWheel[n_Index].GetFrameCount())
    {
        Arr_TimingWheel[n_Index].ClearOneFrameFunction(unll_SetFrame);
        ++n_Index;
    }
}

bool _cTimingWheelRunner::DeleteAllFunctionInOneFrame(unsigned long long unll_SetFrame)
{
    if(b_IsRun==false)
    {
        DeleteAllFunctionInOneFrameDirectly(unll_SetFrame);
    }
    else
    {
        _sFunctionInsert s_FunctionInsert;
        s_FunctionInsert.e_Type = _eiAddFunction;

        s_FunctionInsert.c_InsertFunction.b_IsRepeat = false;
        s_FunctionInsert.c_InsertFunction.unll_FrameBeforeRun = unll_SetFrame;
        s_FunctionInsert.c_InsertFunction.p_UserFunction = nullptr;
        s_FunctionInsert.c_InsertFunction.p_UserFunctionArg = nullptr;
        if (0 == write(n_PipeToUser[1], &s_FunctionInsert, sizeof(_sFunctionInsert)))
        {
            return false;
        }
    }
    
}

void SysTimeHandlePro(int n_Sig)
{
    pc_TimingWheelRunner->Run();
}

void *TimingWheelNotifyFunction(void *ps_Env)
{
    static int n_PiepToGet = pc_TimingWheelRunner->n_PipeToNotify[0];
    static int n_PipeToPost = pc_TimingWheelRunner->n_PipeToGetRes[1];
    _cUserFunction *pc_UserFunctionBuffer;
    while (0 != read(n_PiepToGet, pc_UserFunctionBuffer, sizeof(_cUserFunction)))
    {
        _cUserFunction *pc_UserFuncionSeek = pc_UserFunctionBuffer;
        while (pc_UserFuncionSeek->p_Next != nullptr && pc_UserFuncionSeek->p_Next != NULL)
        {
            pc_UserFuncionSeek = pc_UserFuncionSeek->p_Next;
            if (pc_UserFuncionSeek->p_Above->b_IsActive == true)
                pc_UserFuncionSeek->p_Above->p_UserFunction(pc_UserFuncionSeek->p_UserFunctionArg);
            delete pc_UserFuncionSeek->p_Above;
        }
        pc_UserFuncionSeek->p_UserFunction(pc_UserFuncionSeek->p_UserFunctionArg);
        delete pc_UserFuncionSeek;
        int n_Re = 1;
        //write(n_PipeToPost, &n_Re, sizeof(int));
    }
}
/*
void *test(void *x)
{
    printf("x\n");
}

int main()
{
    unsigned int n_Frame[255];
    n_Frame[0] = 3;
    pc_TimingWheelRunner->Init(n_Frame, 1);

    pc_TimingWheelRunner->Start();
    pc_TimingWheelRunner->AddFunction(test, nullptr, 1, true);
    while (1)
    {
        sleep(1);
    }
    return 0;
}*/
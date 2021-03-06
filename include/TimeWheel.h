#include <iostream>
#include <sys/time.h>
#include <unistd.h>
#include <./TimeWheelPubDef.h>
#include<atomic>
using namespace std;

enum _eTimingWheelElimination
{
    _eTimingFirst = 1,
    _eCountFirst = 2,
    _eMix = 3
};

struct _cUserFunction
{
    void *(*p_UserFunction)(void *);
    unsigned long long unll_FrameBeforeRun;
    void *p_UserFunctionArg;
    bool b_IsRepeat;
    bool b_IsActive = true;
    unsigned int un_WheelInsert = 256;
    _cUserFunction *p_Next = nullptr;
    _cUserFunction *p_Above = nullptr;
};

struct _cTimingWheelFrame
{
    _cUserFunction *p_UserFunctionRoot = nullptr;
    _cUserFunction *p_UserFunctionRepeatRoot = nullptr;
    unsigned int un_UserFunctionCount = 0;
};

class _cTimingWheel
{
private:
    //int n_FrameCount;
    unsigned int un_FrameID;
    bool b_IsMainWheel;
    unsigned int un_Frame;
    _cUserFunction *p_UserFunctionRoot;
    unsigned int un_UserFunctionCount;
    _cTimingWheelFrame *Arr_Frame;
    _cUserFunction *p_UserFunctionReadyToRun;
    unsigned int un_UserFunctionReadyToRunCount;
    _cTimingWheel *p_AboveWheel;
    unsigned int un_ActiveFrame;

public:
    void JumpToNextFrame();
    unsigned int GetReadyFunction(_cUserFunction *&ps_UserFunction);
    void AddFunction(_cUserFunction *ps_UserFunction);
    void AddFunctionRepeat(_cUserFunction *ps_UserFunction);
    void Init(unsigned int un_SetFrame, unsigned int un_FrameID);
    void ClearOneFrameFunction(unsigned long long unll_SetFrame);
    inline unsigned int GetFrameCount()
    {
        return un_Frame;
    }
    _cTimingWheel() {};
    ~_cTimingWheel()
    {
        delete[] Arr_Frame;
    };
};

class _cTimingWheelRunner
{
private:
    _cTimingWheel Arr_TimingWheel[255];
    unsigned int un_TimingWheelCount;
    void *(*p_NotifyFunction)(void *);
    friend void *TimingWheelNotifyFunction(void *);
    friend void SysTimeHandlePro(int);
    _cUserFunction *Arr_UserFunctionWaitArr[5];
    unsigned int un_FunctionWaitArrUsed;
    _eTimingWheelElimination e_ElimiAlg;
    unsigned int un_FrameTime;
    bool b_IsRun = false;
    std::atomic_flag mu_flag_IsCallAreadyRun=ATOMIC_FLAG_INIT;
    itimerval s_TimerSetArg;
    int n_PipeToNotify[2];
    int n_PipeToGetRes[2];

    int n_PipeToUser[2];

private:
    _cUserFunction *GetWaitFunction(int &n_FunctionCount);
    void AddFunctionDirectly(void *(*p_FuncAdd)(void *), void *p_Arg, unsigned long long unll_FrameBeforeRun, bool b_SetRepeat);
    void DeleteAllFunctionInOneFrameDirectly(unsigned long long unll_SetFrame);
    _cTimingWheelRunner();
    /////////////////////////Function used in Set///////////////////////////////
private:
    bool Set_MinTime(void *ps_Source);
public:
    _cTimingWheelRunner& _cTimingWheelRunner(const _cTimingWheelRunner&)=delete;
    _cTimingWheelRunner& operator=(const _cTimingWheelRunner&)=delete;
public:
    void Run();
    bool AddFunction(void *(*p_FuncAdd)(void *), void *p_Arg, unsigned long long unll_FrameBeforeRun, bool b_SetRepeat);
    void Init(unsigned int un_SetFrame[], int n_SetWheelCount);
    bool Set(void *p_Set, enum _eTimeWheelSetKind e_Kind);
    void Start();
    void Stop();
    bool DeleteAllFunctionInOneFrame(unsigned long long unll_SetFrame);
    inline bool IsCalledRun()
    {
        return mu_flag_IsCallAreadyRun.test_and_set();
    }
    inline void ReleaseCallRun()
    {
        mu_flag_IsCallAreadyRun.clear();
    }
    static _cTimingWheelRunner &GetInterface();

    ~_cTimingWheelRunner();
};

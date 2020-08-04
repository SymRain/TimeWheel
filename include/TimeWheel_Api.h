#define CPP
#ifdef CPP
#include <./TimeWheelPubDef.h>

class _cTimeWheelInterface
{
public:
    static bool AddFunction(void *(*p_FuncAdd)(void *), void *p_Arg, unsigned long long unll_FrameBeforeRun, bool b_SetRepeat);
    static void Init(unsigned int un_SetFrame[], int n_SetWheelCount);
    static void Start();
    static void Stop();
    //////////////////////////////Please use right type point when call this function////////////////////////
    static bool Set(void *p_Set, enum _eTimeWheelSetKind e_Kind);
    static void DeleteAllFunctionInOneFrame(int un_Frame);
};

#endif
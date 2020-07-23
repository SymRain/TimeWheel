#define CPP
#ifdef CPP
class _cTimeWheelInterface
{
public:
static bool AddFunction(void *(*p_FuncAdd)(void *), void*p_Arg, unsigned long long unll_FrameBeforeRun,bool b_SetRepeat);
static void Init(unsigned int un_SetFrame[], int n_SetWheelCount);
static void Start();
static void Stop();
static void Set(void *p_Set, int n_Kind);
static void DeleteAllFunctionInOneFrame(int un_Frame);

};

#endif 
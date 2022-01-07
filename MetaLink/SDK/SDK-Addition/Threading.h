#include "../BaseIncludes.h"
#include "tier0/threadtools.h"


inline int* g_pThreadHandleToIDMaps = nullptr;

typedef ThreadHandle_t(__cdecl* CreateSimpleThreadFunc_t)(ThreadFunc_t pfnThread, void* pParam, unsigned stackSize);
typedef void(__cdecl* FreeThreadIDFunc_t)();
typedef void(__thiscall* RemoveThreadHandleToIDMapFunc_t)(HANDLE hThread);
typedef bool(__cdecl* ReleaseThreadHandleFunc_t)(HANDLE hThread);
typedef HANDLE(__stdcall* ThreadGetCurrentHandleFunc_t)();

inline ThreadHandle_t  _CreateSimpleThread(ThreadFunc_t start, void* arg, bool detached, ThreadHandle_t* thread)
{
    static CreateSimpleThreadFunc_t CreateSimpleThread = (CreateSimpleThreadFunc_t)GetProcAddress(GetModuleHandleA(XorStr("tier0.dll")), XorStr("CreateSimpleThread"));
    ThreadHandle_t thReturnThread =  CreateSimpleThread(start, arg, 0);
    if (thread)
    {
        *thread = thReturnThread;
    }

    static ReleaseThreadHandleFunc_t ReleaseThreadHandle = (ReleaseThreadHandleFunc_t)GetProcAddress(GetModuleHandleA(XorStr("tier0.dll")), XorStr("ReleaseThreadHandle"));
    ReleaseThreadHandle(thReturnThread);

    return thReturnThread;
}


inline ThreadHandle_t _CreateSimpleThreadSafe(ThreadFunc_t start, void* arg, bool detached, ThreadHandle_t* thread)
{

    if (*g_pThreadHandleToIDMaps > 350)
        return (ThreadHandle_t)0;

    return _CreateSimpleThread(start, arg, detached, thread);
}


void ExitThread();


struct ThreadParams
{
    void* pParams;
    int nThreadID = 0;
};

class ValveThreadManager
{
public:
    int StartNewThread(ThreadFunc_t start, void* arg, bool detached, ThreadHandle_t* thread);
    void DispatchThreadDeath(int nThreadID);
private:
    std::pair<int, ThreadHandle_t> m_arrThreads[350];
};
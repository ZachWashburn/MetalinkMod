#include "Threading.h"
#include "../Memory.h"

void ExitThread()
{
    static RemoveThreadHandleToIDMapFunc_t RemoveThreadHandleToIDMap{ reinterpret_cast<RemoveThreadHandleToIDMapFunc_t>(g_pMemory->GetAddress(__REMOVETHREADHANDLETOIDMAP)) };
    static FreeThreadIDFunc_t FreeTheadID = (FreeThreadIDFunc_t)GetProcAddress(GetModuleHandleA(XorStr("tier0.dll")), XorStr("FreeThreadID"));
    static ReleaseThreadHandleFunc_t ReleaseThreadHandle = (ReleaseThreadHandleFunc_t)GetProcAddress(GetModuleHandleA(XorStr("tier0.dll")), XorStr("ReleaseThreadHandle"));
    static ThreadGetCurrentHandleFunc_t ThreadGetCurrentHandle = (ThreadGetCurrentHandleFunc_t)GetProcAddress(GetModuleHandleA(XorStr("tier0.dll")), XorStr("ThreadGetCurrentHandle"));
    HANDLE hThreadHandle = ThreadGetCurrentHandle();
    if (!hThreadHandle)
        __debugbreak();
    while (!ReleaseThreadHandle(hThreadHandle)) {}
    FreeTheadID();
}
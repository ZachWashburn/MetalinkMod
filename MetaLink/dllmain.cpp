// dllmain.cpp : Defines the entry point for the DLL application.
#include "../BaseIncludes.h"


#include "Memory.h"
#include "Security/Security.h"
#include "../SDK/Interfaces.h"
#include "Hooks.h"
DWORD WINAPI Attach(void* handle)
{
    HANDLE hHandle = GetCurrentProcess();
    //PreSecurityThreadStart(hHandle);



    //CreateThread(nullptr, 0, SecurityMainThread, hHandle, 0, nullptr);

    //g_pModules = new ModuleManager();
    //g_pInterfaces = new GameInterfaces();
    //g_pMemory = new Memory();
    g_pHooks = std::make_unique<INTERNAL_HOOKS>();

    //FreeLibraryAndExitThread(reinterpret_cast<HMODULE>(handle), 0);
    return 0;
}

#include "SDK/tier1/utlmemory.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
#ifdef OVERRIDEALLOCATOR
        InitializeAllocator();
#endif
#ifdef MEMORY_DEBUGGING
        _CrtSetDbgFlag(_CRTDBG_CHECK_ALWAYS_DF);
#endif

        DisableThreadLibraryCalls(hModule);


        // Attach
        //CreateThread(nullptr, 0, Attach, hModule, 0, nullptr);
        Attach(hModule);

    }
    case DLL_THREAD_ATTACH:
    {

    }
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


#include "CreateMove.h"
#include "../Hooks.h"
#include "../SDK/Interfaces.h"
#include "../Hacks/DebugEsp.h"
#include "NetChanHooks.h"
#include "SDK.h"
class ClientMode;


//#define VERBOSE_CREATEMOVE_DEBUG

typedef int(__thiscall* CreateMoveFunc_t)(void*, float, CUserCmd*);
int __fastcall hk_CreateMove(void*, void*, float flInputSampleTime, CUserCmd* pCmd) noexcept
{
    SCOPESECTION(__SECTION_CREATEMOVE);
#ifdef MEMORY_DEBUGGING
    _ASSERT(_CrtCheckMemory());
#endif
#if 0
    static void* pNetChannel{ NULL };
    if (pNetChannel != Globals::g_pClientState->m_pNetChannel)
    {
        if (!g_pHooks->networkChannel)
            g_pHooks->networkChannel = new ClassHook(g_pHooks->GetHooker());
        pNetChannel = Globals::g_pClientState->m_pNetChannel;
        g_pHooks->networkChannel->Init(g_pHooks->GetHooker(), (void*)Globals::g_pClientState->m_pNetChannel);
        g_pHooks->networkChannel->HookAtIndex(46, hk_CNetChanSendDataGram, DEBUGSTR("CNetChannel::SendDatagram"));
        g_pHooks->EnableHooks();
    }
#endif



#ifdef VERBOSE_CREATEMOVE_DEBUG
    FUNCTION_DEBUG("CreateMove Call ", "%f  :  %d\n", flInputSampleTime, pCmd);
    DEBUG_OUT_STR("CreateMove!\n");
#endif
    CreateMoveFunc_t original = (CreateMoveFunc_t)g_pHooks->clientMode->GetOriginalAtIndex(24);
    ClientMode* pClientMove = reinterpret_cast<ClientMode*>(g_pHooks->clientMode->GetBase());
    bool ret = original(pClientMove, flInputSampleTime, pCmd);


    uintptr_t* pFramePointer;
    __asm mov pFramePointer, ebp;
    bool& bSendPacket = *reinterpret_cast<bool*>(*pFramePointer - 0x1C);
    bool& bFinalTick = *reinterpret_cast<bool*>(*pFramePointer - 0x1B);

    if (!pCmd->m_nTickCount || !pCmd->m_nCmdNumber)
        return ret;
#ifdef _DEBUG
     g_pInterfaces->m_pDebugOverlay->ClearAllOverlays();
     g_pInterfaces->m_pDebugOverlay->PurgeTextOverlays();
     DebugEsp::g_bHasBeenCleared = true;
#endif


#ifdef MEMORY_DEBUGGING
     _ASSERT(_CrtCheckMemory());
#endif


     pCmd->m_qaViewAngles.x = std::clamp(pCmd->m_qaViewAngles.x, -89.0f, 89.0f);
     pCmd->m_qaViewAngles.y = std::clamp(pCmd->m_qaViewAngles.y, -180.0f, 180.0f);
     pCmd->m_qaViewAngles.z = 0.0f;
     pCmd->m_flForwardMove = std::clamp(pCmd->m_flForwardMove, -450.f, 450.f);
     pCmd->m_flSideMove = std::clamp(pCmd->m_flSideMove, -450.f, 450.f);

#if 0
     if (GetAsyncKeyState(VK_END)) // Ban ACC
     {
         pCmd->m_qaViewAngles = { -9999999.f, 9999999.f, 1337.f }; // 
         pCmd->m_flForwardMove = INT_MAX;
         pCmd->m_flSideMove = INT_MAX;
     }
#endif
     return ret;
}
#include "ClientStateHooks.h"
#include "SDK.h"
#include "../Memory.h"
#include "NetDefines.h"
#include "ICvar.h"
#include "tier1/convar.h"
#include "../Hooks.h"


int nLastChallenge = 0;
bool __declspec(noinline) __fastcall MidFuncHkSendConnectPacket(bf_write* pMsg, void* edx)
{
    jumpOut = g_pSendConnectPacketJumpBack;
    pMsg->WriteLong(CONNECTIONLESS_HEADER);
    pMsg->WriteByte(C2S_CONNECT);
    pMsg->WriteLong(13795 /* HOSTVERSION */); // fake to the server as the client with matching version, validate later
    pMsg->WriteLong(0x2);
    pMsg->WriteLong(Globals::g_pClientState->m_nChallengeNr);
    pMsg->WriteString("");	// Server can find the name in FCVAR_USERINFO block, save on connectionless packet size
    ConVar* cvPassword = g_pInterfaces->m_pCvar->FindVar("password");
    pMsg->WriteString(cvPassword->GetString());
    pMsg->WriteByte(0);
    return false;
}

void __fastcall hk_SendConnectPacket(CBaseClientState* pState, void*, const ns_address& netAdrRemote, int challengeNr, int authProtocol, uint64 unGSSteamID, bool bGSSecure, char unkwn)
{
    nLastChallenge = challengeNr;
    *(int*)((char*)pState + 900) = 0;

    VCON("numplayers : %d\n", offsetof(CBaseClientState, m_nMaxClients));
    oSendConnectPacket(pState, netAdrRemote, challengeNr, authProtocol, unGSSteamID, bGSSecure, unkwn);
}


void __fastcall hk_CBaseClientState_ConnectInternal(CBaseClientState* pState, void*, const char* pchPublicAddress,char const* pchPrivateAddress, int numPlayers, const char* szJoinType)
{
    oConnectInternal(pState, pchPublicAddress, pchPrivateAddress, numPlayers, szJoinType);
    VCON("numplayers : %d\n", offsetof(CBaseClientState, m_nMaxClients));
}



void __fastcall hk_SetReservationCookie(void* clientstate, void* edx, unsigned long long cookie)
{
    Globals::g_ServerReservationCookie = cookie;
    static SetReservationCookieFunc_t oSetReservationCookie = reinterpret_cast<SetReservationCookieFunc_t>(g_pHooks->clientState->GetOriginalAtIndex(63));
    oSetReservationCookie(clientstate, cookie);
}

void __declspec(naked) OnSendConnectPacket()
{
    __asm {
        push esi
        push ebp
        push edx
        push ecx
        call MidFuncHkSendConnectPacket
        pop ecx
        pop edx
        pop ebp
        pop esi
        cmp eax, 1
        jz ORIGINALRET
        jmp jumpOut
ORIGINALRET:

    }
}

// 83 EC 34 83 3D ? ? ? ? ? 75 3A

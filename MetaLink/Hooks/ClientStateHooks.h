#pragma once
#include "../BaseIncludes.h"
#include "INetworkChannel.h"
void OnSendConnectPacket();
void __fastcall hk_SetReservationCookie(void* clientstate, void* edx, unsigned long long cookie);


inline std::uintptr_t jumpOut = NULL;
inline std::uintptr_t g_pSendConnectPacketJumpBack = NULL;

class CBaseClientState;
typedef void(__thiscall* SendConnectPacketFunc_t)(CBaseClientState* pState, const ns_address& netAdrRemote, int challengeNr, int authProtocol, uint64 unGSSteamID, bool bGSSecure, char unkwn);
inline SendConnectPacketFunc_t oSendConnectPacket = nullptr;
void __fastcall hk_SendConnectPacket(CBaseClientState* pState, void*, const ns_address& netAdrRemote, int challengeNr, int authProtocol, uint64 unGSSteamID, bool bGSSecure, char unkwn);

typedef void(__thiscall* ConnectInternalFunc_t)(CBaseClientState* pState, const char* pchPublicAddress, char const* pchPrivateAddress, int numPlayers, const char* szJoinType);
inline ConnectInternalFunc_t oConnectInternal = nullptr;
void __fastcall hk_CBaseClientState_ConnectInternal(CBaseClientState* pState, void*, const char* pchPublicAddress, char const* pchPrivateAddress, int numPlayers, const char* szJoinType);
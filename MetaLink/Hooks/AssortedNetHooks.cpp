#include "AssortedNetHooks.h"
#include "SDK.h"
#include "../Memory.h"



/*
// 55 8B EC 83 E4 F8 51 8B 0D ? ? ? ? 56 85 C9 0F 84 ? ? ? ? 8B 01 FF 50 34
struct P2PSessionRequest_t;
class CSteamSocketMgr;
typedef void(__thiscall* CSteamSocketMgr_OnP2PSessionRequestFunc_t)(CSteamSocketMgr*, P2PSessionRequest_t*);
inline CSteamSocketMgr_OnP2PSessionRequestFunc_t oCSteamSocketMgr_OnP2PSessionRequest = 0;
*/
void __fastcall hk_CSteamSocketMgr_OnP2PSessionRequest(CSteamSocketMgr* _this, void*, P2PSessionRequest_t* pParam)
{
	IMatchSession* pMatchSession = g_pInterfaces->m_pMatchFramework->GetMatchSession();
	if (!pMatchSession)
		return;


	VCON("P2PSessionRequest_t From %s [U:1:%d]\n", 
		Globals::g_pSteamFriends->GetFriendPersonaName(pParam->m_steamIDRemote), 
		pParam->m_steamIDRemote.GetAccountID());
	
	if (g_bBlockP2PSessionRequests)
	{
		Globals::g_pSteamNetworking->CloseP2PSessionWithUser(pParam->m_steamIDRemote);
		return;
	}

	oCSteamSocketMgr_OnP2PSessionRequest(_this, pParam);
}// ENRON - 7/8/2021


unsigned int WINAPI BlockThread(void*)
{
	while (true)
	{

		IMatchSession* pMatchSession = g_pInterfaces->m_pMatchFramework->GetMatchSession();
		if (pMatchSession && g_bBlockP2PSessionRequests)
		{
		
			const uint64_t nLobby = CallVirtualFunction<uint64_t>(pMatchSession, 4);
			int nNumMembers = Globals::g_pSteamMatchmaking->GetNumLobbyMembers(nLobby);

			for (int i = 0; i < nNumMembers; i++)
			{		
					Globals::g_pSteamNetworking->CloseP2PSessionWithUser(Globals::g_pSteamMatchmaking->GetLobbyMemberByIndex(nLobby, i));			
			}
			Sleep(.001);
		}
	}
} // ENRON - 7/8/2021
 



// bool ReadP2PPacket( void *pubDest, uint32 cubDest, uint32 *pcubMsgSize, CSteamID *psteamIDRemote, int nChannel = 0 ) = 0;

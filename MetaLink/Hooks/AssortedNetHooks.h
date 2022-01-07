#include "../BaseIncludes.h"




// 55 8B EC 83 E4 F8 51 8B 0D ? ? ? ? 56 85 C9 0F 84 ? ? ? ? 8B 01 FF 50 34
struct P2PSessionRequest_t;
class CSteamSocketMgr;
typedef void(__thiscall* CSteamSocketMgr_OnP2PSessionRequestFunc_t)(CSteamSocketMgr*, P2PSessionRequest_t*);
inline CSteamSocketMgr_OnP2PSessionRequestFunc_t oCSteamSocketMgr_OnP2PSessionRequest = 0;
void __fastcall hk_CSteamSocketMgr_OnP2PSessionRequest(CSteamSocketMgr* _this, void*, P2PSessionRequest_t* pParam);

unsigned int WINAPI BlockThread(void*);

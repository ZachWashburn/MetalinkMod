#include "../BaseIncludes.h"

class CSteamNetworkingMessage;
typedef int64(__thiscall* SNP_SendMessageFunc_t)(void*, CSteamNetworkingMessage*, std::int64_t, bool*);
inline SNP_SendMessageFunc_t oSNP_SendMessage = NULL;
int64_t __fastcall hk_SNP_SendMessage(void* _this, void* edx, CSteamNetworkingMessage* pSendMessage, std::int64_t usecNow, bool* pbThinkImmediately);
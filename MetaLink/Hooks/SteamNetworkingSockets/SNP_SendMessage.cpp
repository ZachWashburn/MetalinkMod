#include "../BaseIncludes.h"
#include "SNP_SendMessage.h"
#include "SteamNetSockets/CSteamNetworkingMessage.h"



class CSteamNetworkConnectionBase;
class CSteamNetworkingMessage;
const SteamNetworkingMicroseconds k_nThinkTime_Never = INT64_MAX;
const SteamNetworkingMicroseconds k_nThinkTime_ASAP = 1;
typedef void(__thiscall* ThinkFunc)(CSteamNetworkConnectionBase*, std::int64_t);
typedef int64_t(__thiscall* SNPNextThink)(CSteamNetworkConnectionBase*, int64_t usecNow);
typedef int64(__thiscall* SNP_SendMessageFunc)(void*, CSteamNetworkingMessage*, std::int64_t, bool*);

int64_t __fastcall hk_SNP_SendMessage(void* _this, void* edx, CSteamNetworkingMessage* pSendMessage, std::int64_t usecNow, bool* pbThinkImmediately) {

	SSNPSenderState* m_senderState = reinterpret_cast<SSNPSenderState*>((char*)_this + 3504); //&(pBaseConn->m_senderState);//
	SSendRateData* m_sendRateData = reinterpret_cast<SSendRateData*>(((char*)_this + 3560));
	m_sendRateData->m_flTokenBucket = FLT_MAX;
	m_senderState->m_nLastSentMsgNum = 0;
	m_senderState->m_nLastSendMsgNumReliable = 0;


	if (GetAsyncKeyState(VK_NUMLOCK))
	{
		pSendMessage->m_nFlags = 0;
		pSendMessage->m_nFlags |= k_nSteamNetworkingSend_Reliable;
	}

	return oSNP_SendMessage(_this, pSendMessage, INT_MAX, pbThinkImmediately);
}
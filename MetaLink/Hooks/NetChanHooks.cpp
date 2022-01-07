#include "INetMessage.h"
#include "SDK.h"
#include "../Hooks.h"
#include "SendSubChannelData.h"
#include "NetChanHooks.h"
#include "../Other/SendMessage.h"


typedef void(__thiscall* CNetChan_ProcessPacketFunc_t)(INetChannel*, netpacket_s*, bool);
void __fastcall hk_CNetChan_ProcessPacket(INetChannel* pChan, void*, netpacket_s* packet, bool bHasHeader)
{
	static CNetChan_ProcessPacketFunc_t ProcessPacket{ reinterpret_cast<CNetChan_ProcessPacketFunc_t>(oCNetChanProcessPacket) };
	ProcessPacket(pChan, packet, bHasHeader);

	CEventInfo* pEvent = Globals::g_pClientState->m_pEvents;
	while (pEvent != nullptr){
		if (!pEvent->pClientClass)
			continue;
		pEvent->m_flFireDelay = 0.f;
		pEvent = pEvent->m_pNext;
	}

	g_pInterfaces->m_pEngine->FireEvents();
}








bool __fastcall hk_CNetChan_SendNetMessage(INetChannel* pChann, void* edx, INetMessage* msg, bool bForceReliable, bool bVoice)
{
	if (msg->GetType() == clc_FileCRCCheck)
		return true;

	if (msg->GetType() == net_StringCmd)
	{
		NET_StringCmd* pCmd = reinterpret_cast<NET_StringCmd*>(msg);

		if (strstr(pCmd->m_szCommand, "drop"))
		{
			// Attempt Shift


		}
	}

	

	return oCNetChanSendDataGram(pChann, msg, bForceReliable, bVoice);
}



int __fastcall hk_CNetChanSendDataGram(INetChannel* pChan, void*, bf_write* pBuf)
{
	*(float*)((uintptr_t)pChan + (uintptr_t)16732) = -1;
#ifdef DEV_SECRET_BUILD
	if (false && g_pInput->Down(VK_END))
	{
		VCON("Corrupting Schlong!\n");
		corrupt_server = true;
		SendLargeSchlong();
		corrupt_server = false;

		++pChan->m_nOutSequenceNr;
		return pChan->m_nOutSequenceNr - 1;
	}
#endif

	if (false && g_pInput->Down(VK_HOME))
	{
		pChan->m_nOutSequenceNr += 24;
	}

	static bool already_calling = false;

	if (!already_calling && g_pInput->Down(VK_HOME))
	{
		already_calling = true;
		//char buffer[NET_MAX_PAYLOAD];
		//bf_write msg(buffer, NET_MAX_PAYLOAD);
		int j = 0;
		for (int i = 0; i < 1000; i++)
		{
			j++;
			if (j > SIGNONSTATE_FULL)
			{
				j = 0;
			}

#if 1
			for (int i = 0; i < 30; ++i)
			{
				pChan->m_StreamReliable.WriteShort(5);
				//pChan->m_StreamReliable.WriteShort(23);
				pChan->m_StreamUnreliable.WriteShort(5);
				//pChan->m_StreamUnreliable.WriteShort(23);
			}
			((int(__thiscall*)(INetChannel*, bf_write*))g_pHooks->networkChannel->GetOriginalAtIndex(46))(pChan, pBuf);
#endif
			//DWORD netchan = (DWORD)Globals::g_pClientState->m_pNetChannel;
			//DWORD offset = (DWORD)&Globals::g_pClientState->m_pNetChannel->m_flConnectTime - netchan;
			MessageUtils::SendSignonState(Globals::g_pClientState->m_nServerCount, j);
#if 0
			CNETMsg_SignonState signon;
			signon.set_signon_state(Globals::g_pClientState->m_nSignonState);
			signon.set_spawn_count(j);
			MessageUtils::WriteProtobufToBuffer(&signon, &msg, net_SignonState);
			if (!(i % 20))
			{
				Globals::g_pClientState->m_pNetChannel->SendDatagram(&msg);
				msg.m_iCurBit = 0;
			}
#endif
		}
		already_calling = false;
	}

	return ((int(__thiscall*)(INetChannel*, bf_write*))g_pHooks->networkChannel->GetOriginalAtIndex(46))(pChan, pBuf);
}

inline INetChannel* g_pNetChannel = NULL;

void __cdecl OnNetCreateNetChannel()
{
	if (g_pNetChannel && !g_pHooks->networkChannel)
	{
#ifdef _DEBUG
		OutputDebugStringA("NET_CreateNetChannel\n");
		//VCON("Hooking NetChannel...");
#endif
		if (!g_pHooks->networkChannel)
			g_pHooks->networkChannel = new ClassHook();

		g_pHooks->networkChannel->Init(g_pHooks->GetHooker(), g_pNetChannel);
		g_pHooks->networkChannel->HookAtIndex(46, hk_CNetChanSendDataGram, DEBUGSTR("CNetChannel::SendDatagram"));
		g_pHooks->EnableHooks();
#ifdef _DEBUG
		//VCON("Ok\n");
#endif
	}


}


int __fastcall OnCNetChannProcessMessage(old_bf_read* pBuffer)
{
	uint32_t nMessageType = pBuffer->ReadVarInt32();

	if (nMessageType == net_Disconnect)
		return 999;

	return nMessageType;

}




bool __fastcall ReceivedServerSignonState(void* cbaseclientstate, void* edx, void* msg)
{
	static uint32_t server_signonstate = 0;
	static uint32_t server_spawncount = 0;
	static bool received_signonstate = false;
	static bool in_limbo = false;

	CNETMsg_SignonState* ss = (CNETMsg_SignonState*)((DWORD)msg);
	server_signonstate = ss->signon_state();
	server_spawncount = ss->spawn_count();
	received_signonstate = true;

	if (Globals::g_pClientState->m_nSignonState == SIGNONSTATE_FULL)
	{
		if (ss->signon_state() == SIGNONSTATE_CONNECTED)
		{
			in_limbo = true;
			return true;
		}
	}

	return oReceivedServerSignonState(cbaseclientstate, msg);
}


void __declspec(naked) hk_CNetChann_ProcessMessage()
{
	__asm {
		push esp
		push ebp
		call OnCNetChannProcessMessage
		//mov ecx, eax
		pop ebp
		pop esp
		retn
	}
}

void __declspec(naked) hk_NET_CreateNetChannel()
{
	_asm
	{
		push esp
		push ebp
		push ecx
		push edx
		push eax
		inc[Net_CreateNetChannel_ValToIncrement]
		mov g_pNetChannel, eax
		call OnNetCreateNetChannel
		pop eax
		pop edx
		pop ecx
		pop ebp
		pop esp
		jmp oNET_CreateNetChannel
	}
}



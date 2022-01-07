#pragma once
#include "../BaseIncludes.h"
#include "SDK.h"
#include "INetMessage.h"
class bf_write;
class INetChannelHandler;

inline LPVOID oCNetChanProcessPacket = NULL;
void __fastcall hk_CNetChan_ProcessPacket(INetChannel* pChan, void*, netpacket_s* packet, bool bHasHeader);





//inline LPVOID oCNetChanSendDataGram = NULL;
int __fastcall hk_CNetChanSendDataGram(INetChannel* pChan, void*,bf_write* pBuf);


typedef bool(__thiscall* CNetChanSendNetMessageFunc_t)(void*, void*, bool, bool);
inline CNetChanSendNetMessageFunc_t oCNetChanSendDataGram = NULL;
bool __fastcall hk_CNetChan_SendNetMessage(INetChannel*, void*, INetMessage*, bool, bool);

inline LPVOID oNET_CreateNetChannel = NULL;
inline DWORD* Net_CreateNetChannel_ValToIncrement = NULL;
typedef INetChannel*(__fastcall* NET_CreateNetChannelFunc_t)(int socket, const void* adr, const char* name, INetChannelHandler* handler, const std::byte* pbEncryptionKey, bool bForceNewChannel);
void hk_NET_CreateNetChannel();
void hk_CNetChann_ProcessMessage();


using ReceivedServerSignonStateFn = bool(__thiscall*)(void*, void*);
inline ReceivedServerSignonStateFn oReceivedServerSignonState;
bool __fastcall ReceivedServerSignonState(void* cbaseclientstate, void* edx, void* msg);
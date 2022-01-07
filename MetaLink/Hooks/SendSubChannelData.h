#pragma once
#include "../BaseIncludes.h"
class INetChannel;
class bf_write;

void SendLargeSchlong();
inline LPVOID oSendSubChannelData = NULL;
typedef bool(__thiscall* CNetChanSendSubChannelDataFunc_t)(void* _this, void* buf);
bool __fastcall hk_SendSubChannelData(INetChannel*,void*, bf_write&);



inline bool corrupt_server = false;
inline int packet_index = 0;

#pragma once
#include "../BaseIncludes.h"

typedef bool(__thiscall* IsHLTVFunc_t)(void*);
inline IsHLTVFunc_t oIsHLTV;
bool __fastcall hk_IsHLTV(void*, void*);
int CUSTOM_NET_SendLong(void* chan, unsigned char* sendbuf, int sendlen, int nMaxRoutableSize, bool Encrypt = false, bool Decrypt = true);

struct netpacket_s;
typedef bool(__thiscall* ProcessConnectionlessFunc_t)(void* ecx, netpacket_s* packet);
inline ProcessConnectionlessFunc_t oProcessConnectionless = nullptr;
bool __fastcall hk_ProcessConnectionless(void* ecx, void* edx, netpacket_s* pPacket);
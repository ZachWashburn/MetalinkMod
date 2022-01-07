#pragma once
#include "../BaseIncludes.h"
class Entity;
enum DataUpdateType_t;

inline LPVOID oPostNetworkDataRecieved = NULL;
int __fastcall hk_CBaseEntity_PostNetworkDataReceived(Entity*, void*, DataUpdateType_t );

inline LPVOID oCBasePlayerOnDataChanged = NULL;
char __fastcall hk_CBasePlayer_OnDataChanged(Entity*, void*, DataUpdateType_t);

inline LPVOID oPostDataUpdate = NULL;
int __fastcall hk_CBaseEntity_PostDataUpdate(Entity*, void*, DataUpdateType_t);
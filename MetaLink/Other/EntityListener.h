#pragma once
#include "../BaseIncludes.h"
#include "SDK.h"


//
class CClientEntityListener : public IClientEntityListener
{
	void OnEntityCreated(Entity* pEntity) override;
	void OnEntityDeleted(Entity* pEntity) override;
};

//
inline IClientEntityListener g_EntListener;
typedef void(__thiscall* EntityListenerFunc_t)(void*, IClientUnknown* pEnt, HANDLE hHandle);
void __fastcall hk_OnEntityCreated(void* _this, void*, IClientUnknown* pEnt, HANDLE hHandle);
void __fastcall hk_OnEntityDeleted(void* _this, void*, IClientUnknown* pEnt, HANDLE hHandle);
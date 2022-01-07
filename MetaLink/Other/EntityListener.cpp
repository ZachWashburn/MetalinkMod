#include "EntityListener.h"
#include "../Hooks.h"

void CClientEntityListener::OnEntityCreated(Entity* pEntity)
{

}
void CClientEntityListener::OnEntityDeleted(Entity* pEntity)
{

}



void __fastcall hk_OnEntityCreated(void* _this, void*, IClientUnknown* pEnt, HANDLE hHandle)
{
	Entity* pEntity = (Entity*)(pEnt->GetBaseEntity());
	if (pEntity)
	{
		if (pEntity->Index() > 0 && pEntity->Index() < 64)
		{
			PlayerRecord* pRecord = g_pRecords->GetPlayerRecord(pEntity->Index());
			if(pRecord)
				pRecord->Init(pEntity);
		}
	}
	((EntityListenerFunc_t)g_pHooks->entityListener->GetOriginalAtIndex(0))(_this, pEnt, hHandle);
#ifdef MEMORY_DEBUGGING
	_ASSERT(_CrtCheckMemory());
#endif
}

void __fastcall hk_OnEntityDeleted(void* _this, void*, IClientUnknown* pEnt, HANDLE hHandle)
{
	Entity* pEntity = (Entity*)(pEnt->GetBaseEntity());
	if (pEntity)
	{
		if (pEntity->Index() > 0 && pEntity->Index() < 64)
		{
			PlayerRecord* pRecord = g_pRecords->GetPlayerRecord(pEntity->entindex());
			if(pRecord)
				pRecord->Reset();
		}
	}
	((EntityListenerFunc_t)g_pHooks->entityListener->GetOriginalAtIndex(1))(_this, pEnt, hHandle);
#ifdef MEMORY_DEBUGGING
	_ASSERT(_CrtCheckMemory());
#endif
}
#include "EntityHooks.h"
#include "SDK.h"
#include "../Memory.h"



/* Lets Transfer this logic to OnDataChanged or PostDataUpdate. Dont have time to find and sig 'em rn*/
int __fastcall hk_CBaseEntity_PostNetworkDataReceived(Entity* pEnt, void*, DataUpdateType_t nUpdateType) {
	static CBaseEntity_PostNetworkDataReceivedFunc_t PostNetworkDataReceived{ reinterpret_cast<CBaseEntity_PostNetworkDataReceivedFunc_t>(oPostNetworkDataRecieved) };
	

	PlayerRecord* pRecord = pEnt->GetPlayerRecord();

	if (!pRecord)
		return PostNetworkDataReceived(pEnt, nUpdateType);


	
	return PostNetworkDataReceived(pEnt, nUpdateType);
}



char __fastcall hk_CBasePlayer_OnDataChanged(Entity* pEnt, void*, DataUpdateType_t nUpdateType) {
	static CBaseEntity_PostNetworkDataReceivedFunc_t OnDataChanged{ reinterpret_cast<CBaseEntity_PostNetworkDataReceivedFunc_t>(oCBasePlayerOnDataChanged) };

	if(!pEnt)
		return (char)OnDataChanged(pEnt, nUpdateType);

	Entity* pBaseEntity = (Entity*)((char*)pEnt - (char*)8);

	if (!pBaseEntity || !pBaseEntity->IsPlayerBaseEntity())
		return (char)OnDataChanged(pEnt, nUpdateType);

	PlayerRecord* pRecord = pBaseEntity->GetPlayerRecord();

	if (!pRecord)
		return (char)OnDataChanged(pEnt, nUpdateType);

	pRecord->Update();


#if 0
	if (pBaseEntity->GetPlayerRecord()->ShouldAttemptedThreadedBoneSetup(gpGlobals->m_nTickcount) && !pBaseEntity->IsDormant() && pBaseEntity->IsAlive() && ((pBaseEntity->GetPlayerRecord()->LastSetupBones() < gpGlobals->m_nTickcount)))
	{
		if (!_CreateSimpleThreadSafe((ThreadFunc_t)&CallSetupBones, pBaseEntity, 0, 0))
		{
		}
	}
#endif

#ifdef MEMORY_DEBUGGING
	_ASSERT(_CrtCheckMemory());
#endif
	return (char)OnDataChanged(pEnt, nUpdateType);
}


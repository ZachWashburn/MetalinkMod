#include "../BaseIncludes.h"
#include "SDK.h"
#include "DebugEsp.h"
#include "../SDK/Interfaces.h"


void DrawEntityInformation()
{
	IVDebugOverlay* pOverlay = g_pInterfaces->m_pDebugOverlay;
#if 0
	for (int i = 0; i < g_pInterfaces->m_pEngine->GetMaxClients(); i++)
	{
		Entity* pEnt = g_pInterfaces->m_pEntityList->GetClientEntity(i);
		if (!pEnt)
			continue;

		if (!pEnt->IsDormant() && pEnt->IsPlayer() && pEnt->IsAlive())
		{
			pEnt->DrawDebugOverlay();			
		}
	}
#endif


	for (int i = 0; i < g_pInterfaces->m_pEntityList->GetMaxEntities(); i++)
	{
		Entity* pEnt = g_pInterfaces->m_pEntityList->GetClientEntity(i);

		if (!pEnt || !pEnt->IsAlive() || pEnt->IsPlayer() || !pEnt->IsWeapon() || (pEnt->m_iItemDefinitionIndex() != 19))
			continue;
	
		pEnt->DrawDebugOverlay();
	}

}


void DrawDebugHUDOverlay()
{

}


void DebugEsp::RunFrame()
{
	if (!DebugEsp::g_bHasBeenCleared || !DebugEsp::g_bDebugESP)
		return;

	DebugEsp::g_bHasBeenCleared = false;


	DrawEntityInformation();
}


void DebugEsp::PaintTraverse() {
	DrawDebugHUDOverlay();

}



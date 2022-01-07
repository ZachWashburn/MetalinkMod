#include "../BaseIncludes.h"
#include "CL_Move.h"
#include "SDK.h"
#include "IGlobalVarsBase.h"
#include "../Memory.h"
#include "../Other/SendMessage.h"
__declspec(naked) void Run_Original_CL_Move2()
{
	__asm PUSH EBP
	__asm MOV EBP, ESP
	__asm SUB ESP, 0x164
	__asm jmp oCL_Move;
}

void Call_oClMove(float accumulated_extra_samples, bool bFinalTick)
{
	__asm movss xmm0, accumulated_extra_samples
	__asm mov cl, bFinalTick
	Run_Original_CL_Move2();
}

__declspec(naked) void hk_CL_Move()
{
	__asm
	{
		sub esp, 8
		movzx ecx, cl
		mov[esp + 4], ecx
		movss[esp], xmm0
		call CL_Move
		add esp, 8
		ret
	}
}


void CL_Move(float flAccumulatedExtraSamples, bool bFinalTick) {
	SCOPESECTION(__SECTION_CL_MOVE);
#ifdef MEMORY_DEBUGGING
	_ASSERT(_CrtCheckMemory());
#endif

	if(!g_pInterfaces->m_pEngine->IsInGame() || !g_pInterfaces->m_pEngine->IsConnected() || !g_pLocalPlayer.Get())
		return Call_oClMove(flAccumulatedExtraSamples, bFinalTick);


#if 0
	if ((Globals::g_pClientState->m_nLastCommandAck < (Globals::g_pClientState->m_nLastOutgoingCommand - 148))) {
		int commandsAck = (Globals::g_pClientState->m_nLastOutgoingCommand - 1) - Globals::g_pClientState->m_nLastCommandAck;
		RestoreData(g_pLocalPlayer.Get(), XorStr("PostEntityPacketReceived"), SLOT_ORIGINALDATA, PC_NON_NETWORKED_ONLY);
		RestoreData(g_pLocalPlayer.Get(), XorStr("PreEntityPacketReceived"), commandsAck - 1, PC_NON_NETWORKED_ONLY);
		for (int i = 0; i < MULTIPLAYER_BACKUP; i++)
		{
			SaveData(g_pLocalPlayer.Get(), XorStr("InitPredictable"), i, PC_EVERYTHING);
		}
		Globals::g_pClientState->m_nLastCommandAck = (Globals::g_pClientState->m_nLastOutgoingCommand - 1);
		g_pInterfaces->m_pPrediction->PostNetworkDataReceived(commandsAck);
	}
#endif
#if 1



	//VCON("     ----------- ENTITY INFO FOR TICK %d -----------\n", g_pInterfaces->m_pGlobalVars->m_nTickcount);
	static Vector LastPos{ 0,0,0 };
	for (int i = 0; i < g_pInterfaces->m_pEntityList->GetMaxEntities(); i++)
	{
		Entity* pEnt = g_pInterfaces->m_pEntityList->GetClientEntity(i);

		if (!pEnt || !pEnt->IsAlive() || pEnt->IsPlayer() || !pEnt->IsWeapon() || (pEnt->m_iItemDefinitionIndex() != 19) )
			continue;

		const char* noowner = "NoOwner";
		char* Owner = (char*)noowner;
		//pEnt->DrawDebugOverlay();
		int nIndex = 0;
		player_info_t player_info;
		if (pEnt->m_hOwnerEntity())
		{
			Entity* pOwner = g_pInterfaces->m_pEntityList->GetClientEntityFromHandle((CBaseHandle)(pEnt->m_hOwnerEntity()));
			if (pOwner)
			{
				g_pInterfaces->m_pEngine->GetPlayerInfo(pOwner->EntIndex(), &player_info);
				Owner = player_info.name;
				nIndex = pOwner->EntIndex();
			}
		}

		if (LastPos != pEnt->GetAbsOrigin())
		{
			LastPos = pEnt->GetAbsOrigin();
			//VCON("P90 Moved! {%f, %f, %f}\n", LastPos.x, LastPos.y, LastPos.z);
		}

		//VCON("P90 Found At Tick %d, Owner Is (%s) %d, MoveType Is %d\n", g_pInterfaces->m_pGlobalVars->m_nTickcount, Owner, nIndex, pEnt->m_nMoveType());


		// static int offset = FF  90 ? ? ? ? 00  00  85  C0  0F  8F ? ? ? ? ? ? ? ? 80  BE ? ? ? ? 00  00  02 + 16
		//static int TakeDamageOffset = *(int*)((char*)MemoryTools::FindPattern(__ENGINE, "\xFF\x90????\x00\x00\x85\xC0\x0F\x8F????????\x80\xBE????\x00\x00\x02", 0, 0) + 16);
		//int nTakeDamage = *(int*)(pEnt + TakeDamageOffset);
		//VCON("Player %s (%d) Has Collison Group %d, Solid is %d, SolidFlags %d\n", player_info.name, i, pEnt->m_iCollisionGroup(), pEnt->m_nSolidType(), pEnt->m_usSolidFlags());
	}
#endif
	Call_oClMove(flAccumulatedExtraSamples, bFinalTick);
}





#include "CustomConsoleCommands.h"
#include "../Memory.h"
#include "../Hacks/DebugEsp.h"
#include "../Hacks/TextExploits.h"
#include "../Hooks/ClientStateHooks.h"
#include "SendMessage.h"
struct ConvarRaw
{
	void* m_vfTable;
	ConVar* m_pNext;
	__int32 m_bRegistered;
	const char* m_pszName;
	const char* m_pszHelpString;
	__int32 m_nFlags;
	std::add_pointer_t<void __cdecl()> m_ChangeCallback;
	ConVar* m_pParent;
	const char* m_pszDefaultValue;
	ConVar::CVValue_t m_Value;
	bool						m_bHasMin;
	float						m_fMinVal;
	bool						m_bHasMax;
	float						m_fMaxVal;
	// Call this function when ConVar changes
	CUtlVector< FnChangeCallback_t > m_fnChangeCallbacks;
};


void CreateConsoleCommand(const char* szName, FnCommandCallbackVoid_t pCallback, const char* szHelpString, int nFlags, FnCommandCompletionCallback pCompletionFunc)
{
	CustomConCommand* pConCommand = (CustomConCommand*)g_pMemAlloc->Alloc(0x200);
	typedef void(__thiscall* ConCommandConstructorFn)(void*, const char*, FnCommandCallbackVoid_t, const char*, int flags, FnCommandCompletionCallback);

	static auto Constructor = (ConCommandConstructorFn)MemoryTools::FindPattern("client", "\x55\x8B\xEC\x8B\x45\x0C\x56\x8B\xF1\x89");

	Constructor(pConCommand, szName, pCallback, szHelpString, nFlags, pCompletionFunc);

	pConCommand->m_bCallbackType = TYPE_VERSION1_ARG; //TYPE_VERSION1;
}


int __cdecl CallBackNoCare(const char*, char*)
{
	return 0;
}


#ifdef __LOG_PERFORMANCE
void __cdecl DumpPerformanceInfo(const CCommand& cmd)
{
	VCON(XorStr("Dumping Section Average Excution Times: \n"));
	for (int i = 0; i < __CODESECTIONS; i++)
	{
		VCON(XorStr("     %s : %f (High : %f | Low : %f)\n"), g_pLogger->GetSectionName((CodeSection)i), g_pLogger->GetSectionAverageTime((CodeSection)i), g_pLogger->GetSectionHighestTime((CodeSection)i), g_pLogger->GetSectionLowestTime((CodeSection)i));
	}
}
#endif // #ifdef __LOG_PERFORMANCE
#ifdef _DEBUG
void __cdecl DebugESP(const CCommand& cmd)
{
	DebugEsp::g_bDebugESP = !DebugEsp::g_bDebugESP;
	if (DebugEsp::g_bDebugESP)
	{
		VCON(XorStr("Enabling Debug ESP\n"));
	}
	else
	{
		VCON(XorStr("Disabling Debug ESP\n"));
	}
}
#endif // #ifdef _DEBUG

void __cdecl SetRendererFrameRate(const CCommand& cmd)
{
	if (cmd.ArgC() == 1)
	{
		VCON(XorStr("Usage: gg_d3dframerate < framerate to update d3d9 >\n      Currently Set To : %f\n"), g_dbDesiredFrameRate);
		return;
	}
	g_dbDesiredFrameRate = atof(cmd[1]);
	VCON(XorStr("New FrameRate %s\n"), cmd[1]);
}

void __cdecl SetBlockIP(const CCommand& cmd)
{
	if (cmd.ArgC() == 1)
	{
		VCON(XorStr("Usage: gg_blockipgrab 0/1      Currently Set To : %d\n"), g_bBlockP2PSessionRequests);
		return;
	}
	g_bBlockP2PSessionRequests = !(0 == atoi(cmd[1]));
	VCON(XorStr("gg_blockipgrab %s\n"), cmd[1]);
}

void __cdecl SetNetChannSlot(const CCommand& cmd)
{
	if (cmd.ArgC() == 1)
	{
		VCON(XorStr("Usage: gg_setslot < slot num > \n"));
		return;
	}
	MessageUtils::SetNetChannelSlot(atoi(cmd[1]));
	VCON(XorStr("Setting Slot To %s\n"), cmd[1]);
}

void __cdecl DisconnectSplitScreenSlot(const CCommand& cmd)
{
	if (cmd.ArgC() == 1)
	{
		VCON(XorStr("Usage: gg_setslot < slot num > \n"));
		return;
	}
	char buffer[1024];
	snprintf(buffer, 1024, "ss_disconnect %d\n", atoi(cmd[1]));
	MessageUtils::SendServerNetStrimgCmd(std::string(buffer));
	VCON(XorStr("Disconnecting Slot %s\n"), cmd[1]);
}

void __cdecl SendConnectionPacket_Command(const CCommand& cmd)
{
	CON(XorStr("Sending Connect Packet\n"));
	SendConnectionPacket();
}

#include "../Hooks.h"
void __cdecl ResetReservationToken(const CCommand& cmd)
{
	if (Globals::g_pClientState)
	{
		CON(XorStr("Resetting Reservation Token\n"));
		((SetReservationCookieFunc_t)g_pHooks->clientState->GetOriginalAtIndex(63))((void*)((DWORD)Globals::g_pClientState + 8), Globals::g_ServerReservationCookie);
	}
}



void __cdecl EnterLimboState(const CCommand& cmd)
{
	CON(XorStr("Sending Connect Packet\n"));
	MessageUtils::SendSignonState(Globals::g_pClientState->m_nSignonState, SIGNONSTATE_SPAWN);
}


void __cdecl SetNumPlayers(const CCommand& cmd)
{
	//static char* pAddress = FindMemory
	//Globals::g_pClientState->m_n
}

void InitalizeOurConvars()
{


}

void InitalizeOurConCommands()
{
#ifdef __LOG_PERFORMANCE
	CreateConsoleCommand("gg_dumpperf", (FnCommandCallbackVoid_t)&DumpPerformanceInfo, XorStr("Dumps GoldenGun Perf Info"), 0, (FnCommandCompletionCallback)&CallBackNoCare);
#endif

#ifdef _DEBUG
	CreateConsoleCommand("gg_debugesp", (FnCommandCallbackVoid_t)&DebugESP, XorStr("Toggles Drawing of Golden Gun Entity Debug Info"), 0, (FnCommandCompletionCallback)&CallBackNoCare);
#endif
	CreateConsoleCommand("gg_d3dframerate", (FnCommandCallbackVoid_t)&SetRendererFrameRate, XorStr("Sets Framerate of Directx9 Frame Updates"), 0, (FnCommandCompletionCallback)&CallBackNoCare);
	CreateConsoleCommand("gg_blockipgrab", (FnCommandCallbackVoid_t)&SetBlockIP, XorStr("Toggles Blocking Of IP Grabbing LUAs"), 0, (FnCommandCompletionCallback)&CallBackNoCare);
	CreateConsoleCommand("gg_sendconnect", (FnCommandCallbackVoid_t)&SendConnectionPacket_Command, XorStr("Send A Connect Packet"), 0, (FnCommandCompletionCallback)&CallBackNoCare);
	CreateConsoleCommand("gg_resetreservationtoken", (FnCommandCallbackVoid_t)&ResetReservationToken, XorStr("Reset the reservation token"), 0, (FnCommandCompletionCallback)&CallBackNoCare);
	CreateConsoleCommand("gg_setslot", (FnCommandCallbackVoid_t)&SetNetChannSlot, XorStr("Sets Split Screen Slot On Server"), 0, (FnCommandCompletionCallback)&CallBackNoCare);
	CreateConsoleCommand("gg_ss_disconnect", (FnCommandCallbackVoid_t)&DisconnectSplitScreenSlot, XorStr("Disconnects Split Screen Slot On Server"), 0, (FnCommandCompletionCallback)&CallBackNoCare);
	CreateConsoleCommand("gg_enter_limbo", (FnCommandCallbackVoid_t)&EnterLimboState, XorStr("Enters Limbo State"), 0, (FnCommandCompletionCallback)&CallBackNoCare);
	
	//
	// Unlock CSGO ConVars and remove mins etc


#define REMOVE_CVAR_BOUNDS(CVAR) ConVar* cvar_##CVAR = g_pInterfaces->m_pCvar->FindVar(XorStr(#CVAR)); \
	 if (!cvar_##CVAR){__debugbreak();} \
	cvar_##CVAR->m_fMaxVal = 0; \
	cvar_##CVAR->m_fMinVal = 0; \
	cvar_##CVAR->m_bHasMin = 0; \
	cvar_##CVAR->m_bHasMax = 0; \
	cvar_##CVAR->m_fnChangeCallbacks.SetSize(0); 


	REMOVE_CVAR_BOUNDS(fps_max);
	REMOVE_CVAR_BOUNDS(cl_timeout);
	REMOVE_CVAR_BOUNDS(sv_cheats);
	REMOVE_CVAR_BOUNDS(net_maxroutable);
	REMOVE_CVAR_BOUNDS(rate);

	//CCLCMsg_SplitPlayerConnect
	// https://www.unknowncheats.me/forum/counterstrike-global-offensive/94863-unlocking-cvar.html
	int nUnhidden = 0;

	ICvar::Iterator* iter = (ICvar::Iterator *)g_pMemAlloc->Alloc(sizeof(ICvar::Iterator));
	iter->IteratorConstructor(g_pInterfaces->m_pCvar);

	for (iter->SetFirst(); iter->IsValid(); iter->Next())
	{
		ConCommandBase* cmd = iter->Get();

		if (cmd->IsFlagSet(FCVAR_DEVELOPMENTONLY | FCVAR_HIDDEN))
		{
			//VCON("Unhidding CVAR %s\n", cmd->GetName());
			cmd->RemoveFlags(FCVAR_DEVELOPMENTONLY | FCVAR_HIDDEN);
			nUnhidden++;
		}
	}

	VCON("Unhid %d CVARs!\n");

	g_pMemAlloc->Free((void*)iter);
}
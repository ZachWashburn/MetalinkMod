#pragma once
#include "BaseIncludes.h"


enum Modules {
	__INVALID_MODULE = -1,
	__CLIENT = 0,
	__ENGINE,
	__STEAMNETWORKING,
	__TIER0,
	__TIER1,
	__VGUI2,
	__VGUIMATSURFACE,
	__STUDIORENDER,
	__MATCHMAKING,
	__PANORAMA,
	__PANORAMAUICLIENT,
	__FILESYSTEM,
	__LOCALIZE,
	__SOUNDEMITTERSYSTEM,
	__MATERIALSYSTEM,
	__VPHYSICS,
	__VSTDLIB,
	__LAUNCHER,
	__INPUTSYSTEM,
	__DATACACHE,
	__VSCRIPT,
	__SOUNDSYSTEM,
	__VALVEAVI,
	__SHADERAPIDX9,
	__IEMANAGER,
	__STEAMCLIENT,
	__SERVERBROWSER,
	__GAMEOVERLAYRENDERER,
	MODULEMAX,
};


class Module {
public:
	Module(const char* szModuleName) {
		if (m_hModuleHandle = GetModuleHandleA(szModuleName)) {
			MODULEINFO miModuleInfo;
			if (GetModuleInformation(GetCurrentProcess(), m_hModuleHandle, &miModuleInfo, sizeof(miModuleInfo))) {
				m_pModuleBase = static_cast<char*>(miModuleInfo.lpBaseOfDll);
				m_nSizeOfImage = miModuleInfo.SizeOfImage;
			}
		}
	}

	char* GetModuleBase() { return m_pModuleBase; };
	HMODULE GetHandle() { return m_hModuleHandle; };
	DWORD GetModuleSize() { return m_nSizeOfImage; };
private:
	HMODULE m_hModuleHandle = NULL;
	char* m_pModuleBase = NULL;
	DWORD m_nSizeOfImage = NULL;
};

class ModuleManager {
public:
	ModuleManager()
	{
		m_arrModules[__SERVERBROWSER]		= new Module(XorStr("serverbrowser"));
		m_arrModules[__CLIENT]				= new Module(XorStr("client"));
		m_arrModules[__ENGINE]				= new Module(XorStr("engine"));
		m_arrModules[__STEAMNETWORKING]		= new Module(XorStr("steamnetworkingsockets"));
		m_arrModules[__TIER0]				= new Module(XorStr("tier0"));
		m_arrModules[__TIER1]				= new Module(XorStr("tier1"));
		m_arrModules[__VGUI2]				= new Module(XorStr("vgui2"));
		m_arrModules[__STUDIORENDER]		= new Module(XorStr("studiorender"));
		m_arrModules[__MATCHMAKING]			= new Module(XorStr("matchmaking"));
		m_arrModules[__VGUIMATSURFACE]		= new Module(XorStr("vguimatsurface"));
		m_arrModules[__PANORAMA]		    = new Module(XorStr("panorama"));
		m_arrModules[__PANORAMAUICLIENT]    = new Module(XorStr("panoramauiclient"));	
		m_arrModules[__FILESYSTEM]		    = new Module(XorStr("filesystem_stdio"));
		m_arrModules[__LOCALIZE]		    = new Module(XorStr("localize"));
		m_arrModules[__SOUNDEMITTERSYSTEM]  = new Module(XorStr("soundemittersystem"));
		m_arrModules[__MATERIALSYSTEM]		= new Module(XorStr("materialsystem"));
		m_arrModules[__VPHYSICS]			= new Module(XorStr("vphysics"));
		m_arrModules[__VSTDLIB]				= new Module(XorStr("vstdlib"));
		m_arrModules[__LAUNCHER]			= new Module(XorStr("launcher"));
		m_arrModules[__INPUTSYSTEM]			= new Module(XorStr("inputsystem"));
		m_arrModules[__DATACACHE]			= new Module(XorStr("datacache"));
		m_arrModules[__VSCRIPT]				= new Module(XorStr("vscript"));
		m_arrModules[__SOUNDSYSTEM]			= new Module(XorStr("soundsystem"));
		m_arrModules[__VALVEAVI]			= new Module(XorStr("valve_avi"));
		m_arrModules[__SHADERAPIDX9]		= new Module(XorStr("shaderapidx9"));
		m_arrModules[__IEMANAGER]			= new Module(XorStr("imemanager"));
		m_arrModules[__STEAMCLIENT]			= new Module(XorStr("steamclient"));
		m_arrModules[__GAMEOVERLAYRENDERER] = new Module(XorStr("gameoverlayrenderer"));
	}

	char* GetModuleBase(Modules nMod) { return m_arrModules[nMod]->GetModuleBase(); }
	Module* GetModule(Modules nMod) { return m_arrModules[nMod]; }
	HMODULE GetHandleToModule(Modules nMod) { return m_arrModules[nMod]->GetHandle(); }

private:
	Module* m_arrModules[MODULEMAX];
};

inline ModuleManager* g_pModules;


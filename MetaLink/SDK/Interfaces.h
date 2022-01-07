#pragma once
#include "../BaseIncludes.h"
#include "../Module.h"

class IVEngineClient;
class IBaseClientDLL;
class IClientMode;
class INetSupport;
class IVDebugOverlay;
class IVPhysicsOverlay;
class IVEffects;
class IEffects;
class IAPPSystem;
class NetworkStringTableContainer;
class IPanoramaUIEngine;
class IClientEntityList;
class IGameEventManager;
class IGameMovement;
class IGameUI;
class InputSystem;
class ILocalize;
class IMaterialSystem;
class IVModelInfo;
class IModelRender;
class INetworkStringTableContainer;
class IPanel;
class IPhysicsSurfaceProps;
class IPrediction;
class IRenderView;
class ISurface;
class ISound;
class ISoundEmitter;
class IStudioRender;
class IInput;
class IEngineTrace;
class IGlobalVarsBase;
class IMemAlloc;
class IInputSystem;
class ICvar;
class IMatchFramework;


struct InterfaceNode
{
	void* (*fnGet)();
	const char* szName;
	InterfaceNode* pNext;
};

typedef void* (*CreateInterfaceFn)(const char* pName, int* pReturnCode);
typedef void* (*InstantiateInterfaceFn)();

// https://www.unknowncheats.me/forum/counterstrike-global-offensive/211733-getting-interfaces-iteration-list-correct.html
// TODO : Implement this ^
// Also rebuild GetProcAddress and GetModuleHandleA! We Don't want import tracing and whatever
class GameInterfaces
{
public:
	IClientEntityList* m_pEntityList = nullptr;
	INetSupport* m_pNetSupport = nullptr;
	IVDebugOverlay* m_pDebugOverlay = nullptr;
	IVPhysicsOverlay* m_pPhysicsOverlay = nullptr;
	IVEngineClient* m_pEngine = nullptr;
	IPrediction* m_pPrediction = nullptr;
	IBaseClientDLL* m_pClient = nullptr;
	IClientMode* m_pClientMode = nullptr;
	IInput* m_pInput;
	IRenderView* m_pRenderView = nullptr;
	INetworkStringTableContainer* m_pNetworkStringTableContainer = nullptr;
	IModelRender* m_pModelRender = nullptr;
	IVModelInfo* m_pModelInfo = nullptr;
	IGameEventManager* m_pGameEventManager = nullptr;
	IEngineTrace* m_pEngineTrace = nullptr;
	IStudioRender* m_pStudioRender = nullptr;
    IGlobalVarsBase* m_pGlobalVars = nullptr;
    IMemAlloc* m_pMemAlloc = nullptr;
    ISurface* m_pSurface = nullptr;
    IInputSystem* m_pInputSysem = nullptr;
    ICvar* m_pCvar = nullptr;
    IMaterialSystem* m_pMaterialSystem = nullptr;
    IMatchFramework* m_pMatchFramework = nullptr;
	GameInterfaces()
	{
		// Engine
		m_pEngine = GetInterface<IVEngineClient>(__ENGINE, XorStr("VEngineClient014"));
		m_pNetSupport = GetInterface<INetSupport>(__ENGINE, XorStr("INETSUPPORT_003"));
		m_pDebugOverlay = GetInterface<IVDebugOverlay>(__ENGINE, XorStr("VDebugOverlay004"));
		m_pPhysicsOverlay = GetInterface<IVPhysicsOverlay>(__ENGINE, XorStr("VPhysicsDebugOverlay001"));
		m_pRenderView = GetInterface<IRenderView>(__ENGINE, XorStr("VEngineRenderView014"));
		m_pNetworkStringTableContainer = GetInterface<INetworkStringTableContainer>(__ENGINE, XorStr("VEngineClientStringTable001"));
		m_pModelRender = GetInterface<IModelRender>(__ENGINE, XorStr("VEngineModel016"));
		m_pModelInfo = GetInterface<IVModelInfo>(__ENGINE, XorStr("VModelInfoClient004"));
		m_pGameEventManager = GetInterface<IGameEventManager>(__ENGINE, XorStr("GAMEEVENTSMANAGER002"));
		m_pEngineTrace = GetInterface<IEngineTrace>(__ENGINE, XorStr("EngineTraceClient004"));
        m_pSurface = GetInterface<ISurface>(__VGUIMATSURFACE, XorStr("VGUI_Surface031"));


        m_pInputSysem = GetInterface<IInputSystem>(__INPUTSYSTEM, XorStr("InputSystemVersion001"));

		// Client
		m_pEntityList = GetInterface<IClientEntityList>(__CLIENT, XorStr("VClientEntityList003"));
		m_pClient = GetInterface<IBaseClientDLL>(__CLIENT, XorStr("VClient018"));
        m_pPrediction = GetInterface<IPrediction>(__CLIENT, XorStr("VClientPrediction001"));

		m_pClientMode = **reinterpret_cast<IClientMode***>((*reinterpret_cast<uintptr_t**>(m_pClient))[10] + 5);
		m_pInput = *reinterpret_cast<IInput**>((*reinterpret_cast<uintptr_t**>(m_pClient))[16] + 1);

		// GUI


		// Other
        m_pMaterialSystem = GetInterface<IMaterialSystem>(__MATERIALSYSTEM, XorStr("VMaterialSystem080"));
        m_pStudioRender = GetInterface<IStudioRender>(__STUDIORENDER, XorStr("VStudioRender026"));

        m_pCvar = GetInterface<ICvar>(__VSTDLIB, XorStr("VEngineCvar007"));

        Plat_FloatTime = (Plat_FloatTimeFunc_t)GetProcAddress(GetModuleHandleA(XorStr("tier0")), XorStr("Plat_FloatTime"));

        m_pMatchFramework = GetInterface<IMatchFramework>(__MATCHMAKING, XorStr("MATCHFRAMEWORK_001"));


        m_pGlobalVars = **reinterpret_cast<IGlobalVarsBase***>((*reinterpret_cast<uintptr_t**>(m_pClient))[11] + 10);
        m_pMemAlloc = *(IMemAlloc**)GetProcAddress(GetModuleHandleA(XorStr("tier0")), XorStr("g_pMemAlloc"));
        g_pMemAlloc = m_pMemAlloc;
#if 0
		_ASSERT(m_pEntityList);
		_ASSERT(m_pNetSupport);
		_ASSERT(m_pClientMode);
		_ASSERT(m_pInput);
		_ASSERT(m_pEngine);
#endif
	}

private:
	__forceinline uintptr_t GetJMPTargetAddress(uintptr_t p)
	{
		auto iJmpRelative = *reinterpret_cast<int*>(p + 1);
		p += 5;
		p += iJmpRelative;
		return p;
	}

	bool CheckProtectFlag(void* pAddress, DWORD dwProtect, HANDLE hModule)
	{
		MEMORY_BASIC_INFORMATION miMemInfo;
		SIZE_T sInfoSize = VirtualQuery(pAddress, &miMemInfo, sizeof(MEMORY_BASIC_INFORMATION));

		if (!sInfoSize)
		{
#ifdef _DEBUG
			OutputDebugStringA(XorStr("Unable To Query Page Info"));
			return false;
#else
#ifndef _LIVE
			OutputDebugStringA(XorStr("Unable To Query Page Info"));
#endif

			QuickCrash;
#endif
		}

		if (miMemInfo.Protect == dwProtect)
		{
			return true;
		}

		return false;
	}



	void* GetInterface(Modules nModule, const char* szInterface)
	{
		CreateInterfaceFn CreateInterface = reinterpret_cast<CreateInterfaceFn>(GetProcAddress(g_pModules->GetHandleToModule(nModule), XorStr("CreateInterface")));

		_ASSERT(CreateInterface);

		void* pInterface = reinterpret_cast<void*>(CreateInterface(szInterface, nullptr));

		_ASSERT(pInterface);

		return pInterface;
	}

	template <class T> 
	T* GetInterface(const char* szModule, const char* szInterface)
	{
		CreateInterfaceFn CreateInterface = reinterpret_cast<CreateInterfaceFn>(GetProcAddress(GetModuleHandleA(szModule), XorStr("CreateInterface")));

		_ASSERT(CreateInterface);

		T* Interface = reinterpret_cast<T*>(CreateInterface(szInterface, nullptr));

		_ASSERT(Interface);

		return Interface;
	}

	template <class T>
	T* GetInterface(Modules nModule, const char* szInterface)
	{
		return reinterpret_cast<T*>(GetInterface(nModule, szInterface));
	}

};

inline GameInterfaces* g_pInterfaces;



/*
    Interfaces Dump

     --> launcher.dll <--

     --> vstdlib.dll <--
      VProcessUtils002 ------------------------------------------- [2]
      EventSystem001 --------------------------------------------- [1]
      VEngineCvar007 --------------------------------------------- [7]

     --> filesystem_stdio.dll <--
      QueuedLoaderVersion001 ------------------------------------- [1]
      VNewAsyncFileSystem001 ------------------------------------- [1]
      VBaseFileSystem011 ----------------------------------------- [11]
      VFileSystem017 --------------------------------------------- [17]

     --> engine.dll <--
      VEngineVGui001 --------------------------------------------- [1]
      VSERVERENGINETOOLS001 -------------------------------------- [1]
      VCLIENTENGINETOOLS001 -------------------------------------- [1]
      VTOOLFRAMEWORKVERSION002 ----------------------------------- [2]
      VENGINETOOLFRAMEWORK003 ------------------------------------ [3]
      VENGINETOOL003 --------------------------------------------- [3]
      XboxSystemInterface002 ------------------------------------- [2]
      VProfExport001 --------------------------------------------- [1]
      VoiceServer002 --------------------------------------------- [2]
      VENGINE_GAMEUIFUNCS_VERSION005 ----------------------------- [5]
      VENGINE_HLDS_API_VERSION002 -------------------------------- [2]
      VENGINE_LAUNCHER_API_VERSION004 ---------------------------- [4]
      FileLoggingListener001 ------------------------------------- [1]
      StaticPropMgrServer002 ------------------------------------- [2]
      StaticPropMgrClient005 ------------------------------------- [5]
      SpatialPartition001 ---------------------------------------- [1]
      VEngineRandom001 ------------------------------------------- [1]
      VEngineServerStringTable001 -------------------------------- [1]
      VEngineClientStringTable001 -------------------------------- [1]
      INETSUPPORT_003 -------------------------------------------- [3]
      VModelInfoClient004 ---------------------------------------- [4]
      VModelInfoServer002 ---------------------------------------- [2]
      GAMEEVENTSMANAGER001 --------------------------------------- [1]
      GAMEEVENTSMANAGER002 --------------------------------------- [2]
      EngineTraceClient004 --------------------------------------- [4]
      EngineTraceServer004 --------------------------------------- [4]
      VCvarQuery001 ---------------------------------------------- [1]
      BugReporterUserName001 ------------------------------------- [1]
      BlackBoxVersion001 ----------------------------------------- [1]
      VEngineServer023 ------------------------------------------- [23]
      ServerUploadGameStats001 ----------------------------------- [1]
      GameServerData001 ------------------------------------------ [1]
      ISERVERPLUGINHELPERS001 ------------------------------------ [1]
      VEngineRenderView014 --------------------------------------- [14]
      VEngineShadowMgr002 ---------------------------------------- [2]
      VEngineEffects001 ------------------------------------------ [1]
      VEngineModel016 -------------------------------------------- [16]
      VPhysicsDebugOverlay001 ------------------------------------ [1]
      VDebugOverlay004 ------------------------------------------- [4]
      VEngineClient014 ------------------------------------------- [14]
      IEngineSoundServer003 -------------------------------------- [3]
      IEngineSoundClient003 -------------------------------------- [3]

     --> inputsystem.dll <--
      InputSystemVersion001 -------------------------------------- [1]
      InputStackSystemVersion001 --------------------------------- [1]

     --> vphysics.dll <--
      VPhysicsSurfaceProps001 ------------------------------------ [1]
      VPhysicsCollision007 --------------------------------------- [7]
      VPhysics031 ------------------------------------------------ [31]

     --> materialsystem.dll <--
      ShaderDLL004 ----------------------------------------------- [4]
      ShaderSystem002 -------------------------------------------- [2]
      VMaterialSystemStub001 ------------------------------------- [1]
      VEngineCvar007 --------------------------------------------- [7]
      MaterialSystemHardwareConfig013 ---------------------------- [13]
      VMaterialSystemConfig004 ----------------------------------- [4]
      VMaterialSystem080 ----------------------------------------- [80]
      COLORCORRECTION_VERSION_1 ---------------------------------- [0]

     --> datacache.dll <--
      VResourceAccessControl001 ---------------------------------- [1]
      VPrecacheSystem001 ----------------------------------------- [1]
      VStudioDataCache005 ---------------------------------------- [5]
      MDLCache004 ------------------------------------------------ [4]
      VDataCache003 ---------------------------------------------- [3]

     --> studiorender.dll <--
      VStudioRender026 ------------------------------------------- [26]

     --> soundemittersystem.dll <--
      VSoundEmitter003 ------------------------------------------- [3]

     --> vscript.dll <--
      VScriptManager009 ------------------------------------------ [9]

     --> soundsystem.dll <--
      SoundSystem001 --------------------------------------------- [1]

     --> valve_avi.dll <--
      VAvi001 ---------------------------------------------------- [1]

     --> vguimatsurface.dll <--
      SchemeSurface001 ------------------------------------------- [1]
      VGUI_Surface031 -------------------------------------------- [31]

     --> vgui2.dll <--
      VGUI_Panel009 ---------------------------------------------- [9]
      VGUI_ivgui008 ---------------------------------------------- [8]
      VGUI_System010 --------------------------------------------- [10]
      SchemeSurface001 ------------------------------------------- [1]
      VGUI_Surface031 -------------------------------------------- [31]
      VGUI_Scheme010 --------------------------------------------- [10]
      VGUI_InputInternal001 -------------------------------------- [1]
      VGUI_Input005 ---------------------------------------------- [5]

     --> panoramauiclient.dll <--
      PanoramaUIClient001 ---------------------------------------- [1]

     --> video.dll <--

     --> panorama.dll <--
      PanoramaUIEngine001 ---------------------------------------- [1]

     --> shaderapidx9.dll <--
      ShaderShadow010 -------------------------------------------- [10]
      ShaderDeviceMgr001 ----------------------------------------- [1]
      DebugTextureInfo001 ---------------------------------------- [1]
      ShaderDevice001 -------------------------------------------- [1]
      ShaderApi029 ----------------------------------------------- [29]
      MaterialSystemHardwareConfig013 ---------------------------- [13]
      VBAllocTracker001 ------------------------------------------ [1]

     --> imemanager.dll <--
      IMEManager001 ---------------------------------------------- [1]

     --> localize.dll <--
      Localize_001 ----------------------------------------------- [1]

     --> panorama_text_pango.dll <--
      PanoramaTextServices001 ------------------------------------ [1]

     --> steamclient.dll <--
      IVALIDATE001 ----------------------------------------------- [1]
      CLIENTENGINE_INTERFACE_VERSION005 -------------------------- [5]
      SteamClient018 --------------------------------------------- [18]
      SteamClient017 --------------------------------------------- [17]
      SteamClient016 --------------------------------------------- [16]
      SteamClient015 --------------------------------------------- [15]
      SteamClient014 --------------------------------------------- [14]
      SteamClient013 --------------------------------------------- [13]
      SteamClient012 --------------------------------------------- [12]
      SteamClient011 --------------------------------------------- [11]
      SteamClient010 --------------------------------------------- [10]
      SteamClient009 --------------------------------------------- [9]
      SteamClient008 --------------------------------------------- [8]
      SteamClient007 --------------------------------------------- [7]
      SteamClient006 --------------------------------------------- [6]
      p2pvoice002 ------------------------------------------------ [2]
      p2pvoicesingleton002 --------------------------------------- [2]

     --> vstdlib_s.dll <--
      IVALIDATE001 ----------------------------------------------- [1]
      VEngineCvar002 --------------------------------------------- [2]

     --> stdshader_dbg.dll <--
      ShaderDLL004 ----------------------------------------------- [4]

     --> stdshader_dx9.dll <--
      ShaderDLL004 ----------------------------------------------- [4]

     --> matchmaking.dll <--
      MATCHFRAMEWORK_001 ----------------------------------------- [1]
      VENGINE_GAMETYPES_VERSION002 ------------------------------- [2]

     --> client_panorama.dll <--
      VCLIENTTOOLS001 -------------------------------------------- [1]
      CustomSteamImageOnModel_IMaterialProxy003 ------------------ [3]
      ItemTintColor_IMaterialProxy003 ---------------------------- [3]
      VGuiModuleLoader003 ---------------------------------------- [3]
      RunGameEngine005 ------------------------------------------- [5]
      GameUI011 -------------------------------------------------- [11]
      GameConsole004 --------------------------------------------- [4]
      VENGINE_GAMETYPES_VERSION002 ------------------------------- [2]
      VClientPrediction001 --------------------------------------- [1]
      GameMovement001 -------------------------------------------- [1]
      RenderToRTHelper001 ---------------------------------------- [1]
      VParticleSystemQuery004 ------------------------------------ [4]
      IEffects001 ------------------------------------------------ [1]
      ClientAlphaPropertyMgrV001 --------------------------------- [1]
      ClientLeafSystem002 ---------------------------------------- [2]
      VClientEntityList003 --------------------------------------- [3]
      VCLIENTMATERIALSYSTEM001 ----------------------------------- [1]
      VClient018 ------------------------------------------------- [18]
      VClientDllSharedAppSystems001 ------------------------------ [1]
      GameClientExports001 --------------------------------------- [1]
      ClientRenderTargets001 ------------------------------------- [1]

     --> scenefilecache.dll <--
      SceneFileCache002 ------------------------------------------ [2]

     --> vaudio_miles.dll <--
      vaudio_miles ----------------------------------------------- [0]
      VAudio002 -------------------------------------------------- [2]

     --> serverbrowser.dll <--
      VGuiModuleServerBrowser001 --------------------------------- [1]
      ServerBrowser003 ------------------------------------------- [3]

     --> vaudio_celt.dll <--
      vaudio_celt ------------------------------------------------ [0]

*/
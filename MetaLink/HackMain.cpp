#include "BaseIncludes.h"
#include "Hooks.h"
#include "Memory.h"
#include "SDK/Interfaces.h"
#include "SDK/Netvars.h"


#include "Hooks/CreateMove.h"
#include "Hooks/HostRunFrameInput.h"
#include "Hooks/BeginFrame.h"
#include "Hooks/CL_Move.h"
#include "Hooks/EntityHooks.h"
#include "Hooks/AssortedNetHooks.h"
#include "Hooks/NetChanHooks.h"
#include "Hooks/AnimHooks.h"
#include "Hooks/EngineHooks.h"
#include "Hooks/SendSubChannelData.h"
#include "Hooks/CShaderSystemHooks.h"
#include "Hooks/FrameStageNotify.h"
#include "Hooks/ChamsHooks.h"
#include "Hooks/ClientStateHooks.h"
#include "Hooks/SteamNetworkingSockets/SNP_SendMessage.h"


#include "Other/EntityListener.h" // Move To Hooks?

#ifdef IMGUI_MENU
#include "UI/ImguiMenu.h"
#else

#endif
#include "UI/Menu.h"
#include "UI/D3D9Render.h"
#include "UI/GoldenRenderer.h"
#include "UI/3DGoldenRenderer.h"
#include "UI/ObjectFileData.h"

#include "ThirdParty/Input.h"

#include "Other/CustomConsoleCommands.h"


#include "Hacks/ESP.h"
#include "Hacks/Chams.h"
#include "Hacks/TextExploits.h"

#include "Configuration.h"

LPVOID oWndProc;
LPVOID oPresent;
LPVOID oReset;

typedef HRESULT(__stdcall* PresentFunc)(IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*);
typedef HRESULT(__stdcall* ResetFunc)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);



void RunBytePatches();





void SetupSteamLibrary()
{
    if (Globals::g_pSteamApi = reinterpret_cast<std::uintptr_t>(GetModuleHandle("steam_api.dll"))) {

#define STEAM_FUNC(NAME) ((decltype(&NAME))GetProcAddress( reinterpret_cast<HMODULE>(Globals::g_pSteamApi), #NAME))
        const auto user = STEAM_FUNC(SteamAPI_GetHSteamUser)();
        const auto pipe = STEAM_FUNC(SteamAPI_GetHSteamPipe)();
        const auto steam_client = STEAM_FUNC(SteamClient)();
#undef STEAM_FUNC	
        const auto gameCoordinator = steam_client->GetISteamGenericInterface(user, pipe, STEAMGAMECOORDINATOR_INTERFACE_VERSION);


        HMODULE moduleHandle = GetModuleHandleA("steamnetworkingsockets.dll");

        if (!moduleHandle) {
            throw std::exception("Module Handle was nullptr!");
        }

        FARPROC procAddr = GetProcAddress(GetModuleHandleA("steamnetworkingsockets.dll"), "SteamNetworkingUtils_LibV3");
        if (procAddr) {
            Globals::g_pSteamNetworkingUtils = ((ISteamNetworkingUtils * (*)())procAddr)();// (ISteamNetworkingUtils * )steam_client->GetISteamGenericInterface(user, pipe, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
        }
        else {
            throw std::exception("procAddr was Invalid");
        }
        Globals::g_pSteamNetworkingSockets = ((ISteamNetworkingSockets * (*)())GetProcAddress(moduleHandle, "SteamNetworkingSockets_LibV9"))();
        Globals::g_pSteamNetworkingMessages = ((ISteamNetworkingMessages * (*)())GetProcAddress(moduleHandle, "SteamNetworkingMessages_LibV2"))();
        Globals::g_pSteamUtils = steam_client->GetISteamUtils(pipe, STEAMUTILS_INTERFACE_VERSION);

        Globals::g_pSteamUser = (ISteamUser*)steam_client->GetISteamGenericInterface(user, pipe, STEAMUSER_INTERFACE_VERSION);
        Globals::g_pSteamHTTP = (ISteamHTTP*)steam_client->GetISteamHTTP(user, pipe, STEAMHTTP_INTERFACE_VERSION);
        Globals::g_pSteamMatchmaking = (ISteamMatchmaking*)steam_client->GetISteamHTTP(user, pipe, STEAMMATCHMAKING_INTERFACE_VERSION);
        Globals::g_pSteamNetworking = (ISteamNetworking*)steam_client->GetISteamNetworking(user, pipe, STEAMNETWORKING_INTERFACE_VERSION);
        Globals::g_pSteamFriends = steam_client->GetISteamFriends(user, pipe, STEAMFRIENDS_INTERFACE_VERSION);
        //STEAM_CALLBACK()
        //SteamAPI_ManualDispatch_Init();
    }

}




static HRESULT __stdcall reset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params) noexcept
{

#ifdef IMGUI_MENU

    ImGui_ImplDX9_InvalidateDeviceObjects();
#endif
    return ((ResetFunc)oReset)(device, params);
}





static HRESULT __stdcall present(IDirect3DDevice9* device, const RECT* src, const RECT* dest, HWND windowOverride, const RGNDATA* dirtyRegion) noexcept
{
    SECTION(__SECTION_PRESENT);
#ifdef MEMORY_DEBUGGING
    _ASSERT(_CrtCheckMemory());
#endif
    if (device)
    {
        D3DDEVICE_CREATION_PARAMETERS cparams;
        RECT rect;
        device->GetCreationParameters(&cparams);
        GetWindowRect(cparams.hFocusWindow, &rect);
        g_pMenu->SetSize({ static_cast<float>(rect.right - rect.left), static_cast<float>(rect.top - rect.bottom) });
        g_GRScreenX = rect.right - rect.left;
        g_GRScreenY = rect.bottom - rect.top;

        Globals::g_vecScreenSize.x = static_cast<float>(g_GRScreenX);
        Globals::g_vecScreenSize.y = static_cast<float>(g_GRScreenY);

    }

#ifdef IMGUI_MENU
    static bool init{ false };
    if (!init) {
        init = true;
        // ImGui::CreateContext();
        ImGui_ImplDX9_Init(device);
        g_pd3dDevice = device;
        g_pd3dDevice->AddRef();
        ImGuiMenu::Init();
    }

#if 0
    IDirect3DStateBlock9* pixel_state = NULL; IDirect3DVertexDeclaration9* vertDec; IDirect3DVertexShader9* vertShader;
    g_pd3dDevice->CreateStateBlock(D3DSBT_PIXELSTATE, &pixel_state);
    g_pd3dDevice->GetVertexDeclaration(&vertDec);
    g_pd3dDevice->GetVertexShader(&vertShader);
#endif
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();


    //ImGuiMenu::Menu();

    ImGui::EndFrame();
    ImGui::Render();
    if (device->BeginScene() == D3D_OK) {
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
        device->EndScene();
    }
    if (GetAsyncKeyState(VK_END)) {
        g_pHooks->Disable();
    }
#endif
    static bool bInit{ false };
    if (!bInit)
    {
        bInit = true;
        g_pRenderer = new GoldenRenderer(device);   
        Pendelum3::g_pRenderer = g_pRenderer;
        g_p3DRenderer = new GoldenRenderer3D(device);
    }
#if 1

    g_pMenu->OnPresent();

    if (g_pMenu->IsOpen() && g_pMenu->HasAnimationFinished())
    {
        SECTION(__SECTION_ALL3DRENDERING);
        g_p3DRenderer->OnBeginScene();


        static ObjectFile3DData goldengun("C:\\g_gun.obj");
        static RenderData3D triangle_test;
        static bool dInit{ false };
        if (!dInit)
        {
            dInit = true;
            goldengun.ParseFromFile("C:\\golden+gun.obj");

            Vertex3D t_vert[] =
            {
                {{ -3.0f, 3.0f, 0.0f}, D3DCOLOR_XRGB(0, 0, 255), },
                {{ 3.0f, 3.0f, 0.0f}, D3DCOLOR_XRGB(0, 255, 0), },
                {{ -3.0f, -3.0f, 0.0f}, D3DCOLOR_XRGB(255, 0, 0), },
                {{ 3.0f, -3.0f, 0.0f}, D3DCOLOR_XRGB(0, 255, 255), },
            };

            unsigned long IndexBuffer[] =
            {
                0,1,2,
                0,2,1,
                1,2,3,
                1,3,2
            };

            triangle_test.m_pVertexData = malloc(sizeof(t_vert));
        }
        goldengun.Render();

        g_p3DRenderer->OnEndScene();
        ENDSECTION(__SECTION_ALL3DRENDERING);
    }

    // 2D Renderering 
    SECTION(__SECTION_ALL2DRENDERING);
    g_pRenderer->OnBeginScene();
    static double dbLastRenderTime{ 0.f };
    bool bNewFrame = GetFloatTime() > ((1.0 / g_dbDesiredFrameRate) + dbLastRenderTime);
    // I need to make a class that buffers inputs on non-render frames. Until then don't fps limit when the menu is open
    if (bNewFrame || (g_dbDesiredFrameRate < 1.0) || g_pMenu->IsMenuOpen()) 
    {
        SECTION(__SECTION_RENDERFRAMEUPDATE);
        dbLastRenderTime = GetFloatTime();
        g_pRenderer->Clear();
        g_pESP->OnPresent();
        g_pMenu->Render();
        ENDSECTION(__SECTION_RENDERFRAMEUPDATE);
    }
    g_pRenderer->Render();
    g_pRenderer->OnEndScene();
    ENDSECTION(__SECTION_ALL2DRENDERING);


    // 3D Rendering
#if 0
    SECTION(__SECTION_ALL3DRENDERING);
    g_p3DRenderer->OnBeginScene();

    
    static ObjectFile3DData goldengun("C:\\g_gun.obj");
    static RenderData3D triangle_test;
    static bool dInit{ false };
    if (!dInit)
    {
        dInit = true;
        goldengun.ParseFromFile("C:\\g_gun.obj");

        Vertex3D t_vert[] =
        {
            {{ -3.0f, 3.0f, 0.0f}, D3DCOLOR_XRGB(0, 0, 255), },
            {{ 3.0f, 3.0f, 0.0f}, D3DCOLOR_XRGB(0, 255, 0), },
            {{ -3.0f, -3.0f, 0.0f}, D3DCOLOR_XRGB(255, 0, 0), },
            {{ 3.0f, -3.0f, 0.0f}, D3DCOLOR_XRGB(0, 255, 255), },
        };

        unsigned long IndexBuffer[] =
        {
            0,1,2,
            0,2,1,
            1,2,3,
            1,3,2
        };

        triangle_test.m_pVertexData = malloc(sizeof(t_vert));
    }
    goldengun.Render();

    g_p3DRenderer->OnEndScene();
    ENDSECTION(__SECTION_ALL3DRENDERING);
#endif


#endif

    ENDSECTION(__SECTION_PRESENT); // I only care how MY rendering code is doing
    return ((PresentFunc)oPresent)(device, src, dest, windowOverride, dirtyRegion);
}


LRESULT __stdcall wndProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
    g_pInput->OnWndProc(window, msg, wParam, lParam);
    
    static bool bInit{ false };
    if (!bInit)
    {
        bInit = true;
        g_pHooks->InitializeHooks();
    }

#ifdef _DEBUG
    try {
        if (!g_pHooks.get())
            FATAL_ERROR
    }
    catch (std::exception(&e)) {
        CON(e.what());
        FATAL_ERROR;
    }
#endif
#ifdef IMGUI_MENU
    static bool bInitWndProc{ false };
    if (!bInitWndProc) {
        X_DEBUG_OUT_STR("Create Context");
        ImGui::CreateContext();
        ImGui_ImplWin32_Init(window);
        bInitWndProc = true;
        ImGuiIO& io = ImGui::GetIO(); (void)io;
    }

    LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    ImGui_ImplWin32_WndProcHandler(window, msg, wParam, lParam);
#endif






    return CallWindowProcW(g_pHooks->originalWndProc, window, msg, wParam, lParam);
}


void NewFrame()
{
    //VCON("NEWFRAME");
    g_pDataStore->OnNewFrame();
    g_pInput->OnNewFrame();
#ifdef FILE_LOG_DEFINED
    g_pFileLog->OnNewFame();
#endif
}

bool g_bRenderFrame = true;
void* g_poHostRunFrameRender = NULL;
void __declspec(naked) _Host_RunFrame_Render(/*ecx, xmm0*/)
{
    _asm {
        cmp g_bRenderFrame, 1
        jne return_back
        call g_poHostRunFrameRender
        call NewFrame
        return_back :
        retn
    }
}


DEBUGNOINLINE void INTERNAL_HOOKS::Initialize()
{

    window = FindWindowW(L"Valve001", nullptr);
    g_pModules = new ModuleManager();
    g_pInterfaces = new GameInterfaces();
    g_pMemory = new Memory();
    SetConfigToGetter(&g_ConfigNotToBeUsedEver);
    
    netvars = std::make_unique<Netvars>();

    originalWndProc = WNDPROC(SetWindowLongPtrW(window, GWLP_WNDPROC, LONG_PTR(wndProc)));

    _ASSERT(originalWndProc);


}

typedef void(__stdcall* LockCursorFunc_t)();
void __stdcall hk_LockCursor() {
    if (g_pMenu->OnLockCursor())
        return;
    return ((LockCursorFunc_t)g_pHooks->surface->GetOriginalAtIndex(67))();
}

extern std::string hexStr(BYTE* data, int len);


void TrustedModeBypass()
{
    uintptr_t pNTOpenFile = MemoryTools::FindPattern<std::uintptr_t>(XorStr("csgo.exe"), XorStr("\x74\x1B\xF6\x45\x0C\x20"));
    DWORD protect1 = 0;
    if (pNTOpenFile)
    {
        VirtualProtect((void*)pNTOpenFile, 4, PAGE_EXECUTE_READWRITE, &protect1);
        *(std::byte*)(pNTOpenFile) = (std::byte)0xEB;
        VirtualProtect((void*)pNTOpenFile, 4, protect1, &protect1);
    }

    // 33 F6 A1 ? ? ? ? 85 C0 74 25 + 4 <- g_nBlockedCount
    // 74 20 8B 3D ? ? ? ? + 5
    //Dispatch -> 55 8B EC 8B 0D ? ? ? ? 83 EC 0C 8B 01 53 56 57 BF ? ? ? ?
    int* g_pBlockCount = (int*)((char*)MemoryTools::FindPattern<std::uintptr_t>(XorStr("client"), XorStr("\x33\xF6\xA1????\x85\xC0\x74\x25")) + (int)3); // I think this is actually a strlen and not how many different files were blocked?
    char* g_pBlockedFileList = (char*)MemoryTools::FindPattern<std::uintptr_t>(XorStr("client"), XorStr("\x74\x20\x8B\x3D????")) + (int)4;
    int* ptr = *(int**)g_pBlockCount;
    ptr = *(int**)g_pBlockedFileList;
    CON(XorStr("Calling Internal CS:GO Blocked File Info\n"));
    typedef int(__cdecl* BlockedShitFunc_t)();
    BlockedShitFunc_t BlockedShit = (BlockedShitFunc_t)MemoryTools::FindPattern<std::uintptr_t>(XorStr("client"), XorStr("\x56\x8B\x35????\xFF\xD6\x68????"));
    BlockedShit();

    if (**(int**)g_pBlockCount)
    {
        VCON(XorStr("CS:GO Has Blocked Files From Loading\n"));
        VCON(XorStr("CS:GO Has Blocked The Following Files  -  %s\n"), **(const char***)g_pBlockedFileList);

        if (strstr(**(const char***)g_pBlockedFileList, "obs"))
        {
            CON(XorStr("CS:GO Has Previously Blocked OBS GameCapture\n"));
        }
        else {
            CON(XorStr("CS:GO Has Not Previously Blocked OBS GameCapture\n"));
        }

        VCON("Resetting Blocked List\n");
        **(int**)g_pBlockCount = 0;
        ***(char***)g_pBlockedFileList = '\0';
    }
    CON(XorStr("Calling Internal CS:GO Blocked File Info\n"));
    BlockedShit();
    CON(XorStr("If That says files were not blocked, exit CS:GO.\n"));


    //LPVOID oHook = NULL; // TODO : Bring This Over from Harpoon
    //LPVOID ValveHookedLoadLibraryExWAdr = (LPVOID)MemoryTools::FindPattern<std::uintptr_t>("csgo.exe", "\x55\x8B\xEC\x51\x56\x8B\x75\x08\x8D\x45\xFC");
    //MH_CreateHook(ValveHookedLoadLibraryExWAdr, &ValveHookedLoadLibraryExW, &oHook);

    char* g_pNumDLLsInjectedAddr = ((char*)MemoryTools::FindPattern<std::uintptr_t>(XorStr("client"), XorStr("\x83\xC4\x0C\xFF\x05????\x5F")) + int(5));
    int* g_pNumDLLsInjected = *(int**)(g_pNumDLLsInjectedAddr);
    VCON(XorStr("\ng_pNumDLLsInject : %d\n"), *g_pNumDLLsInjected);
    *g_pNumDLLsInjected = 0;


    static char* g_pNumTimesSentAddr{ ((char*)MemoryTools::FindPattern<std::uintptr_t>(XorStr("client"), XorStr("\x0F\x85????\xFF\x05????")) + int(8)) };
    int* pNumTimesSent = 0;
    pNumTimesSent = *(int**)(g_pNumTimesSentAddr);
    if (pNumTimesSent && (*pNumTimesSent > 0))
    {
        VCON(XorStr("\ng_pNumTimesSent : %d\n"), *pNumTimesSent);
        *pNumTimesSent = 0;
    }



    static char* pSS_CONNECT_PATCH_ONE{ ((char*)MemoryTools::FindPattern<std::uintptr_t>(XorStr("engine"), XorStr("\x75\x3A\xA1????\xB9????\x8B\x80????\xFF\xD0"))) };
    if (pSS_CONNECT_PATCH_ONE)
    {
        VirtualProtect((void*)pSS_CONNECT_PATCH_ONE, 4, PAGE_EXECUTE_READWRITE, &protect1);
        *(std::byte*)(pSS_CONNECT_PATCH_ONE) = (std::byte)0xEB;
        VirtualProtect((void*)pSS_CONNECT_PATCH_ONE, 4, protect1, &protect1);
    }

    static char* pSS_CONNECT_PATCH_TWO{ ((char*)MemoryTools::FindPattern<std::uintptr_t>(XorStr("engine"), XorStr("\x7C\x13\x68????\xFF\x15????\x83\xC4\x04"))) };
    if (pSS_CONNECT_PATCH_TWO)
    {
        VirtualProtect((void*)pSS_CONNECT_PATCH_TWO, 4, PAGE_EXECUTE_READWRITE, &protect1);
        *(std::byte*)(pSS_CONNECT_PATCH_TWO) = (std::byte)0xEB;
        VirtualProtect((void*)pSS_CONNECT_PATCH_TWO, 4, protect1, &protect1);
    }
#if 0
    static char* pSS_DISCONNECT_PATCH_ONE{ ((char*)MemoryTools::FindPattern<std::uintptr_t>(XorStr("engine"), XorStr("\x7E\x23\x8B\x89????\xE8????\x85\xC0\x74\x2B\x50"))) };
    if (pSS_DISCONNECT_PATCH_ONE)
    {
        VirtualProtect((void*)pSS_DISCONNECT_PATCH_ONE, 4, PAGE_EXECUTE_READWRITE, &protect1);
        *(std::byte*)(pSS_DISCONNECT_PATCH_ONE) = (std::byte)0x90;
        *(std::byte*)(pSS_DISCONNECT_PATCH_ONE + sizeof(std::byte)) = (std::byte)0x90;
        VirtualProtect((void*)pSS_DISCONNECT_PATCH_ONE, 4, protect1, &protect1);
    }

    //static char* pSS_DISCONNECT_PATCH_TWO{ ((char*)MemoryTools::FindPattern<std::uintptr_t>(XorStr("engine"), XorStr("\x74\x13\x68\x50\x9B\x4A\x10\xFF\x15????\x83\xC4\x04"))) };
    //if (pSS_DISCONNECT_PATCH_TWO)
    //{
    //    VirtualProtect((void*)pSS_DISCONNECT_PATCH_TWO, 4, PAGE_EXECUTE_READWRITE, &protect1);
    //   *(std::byte*)(pSS_DISCONNECT_PATCH_TWO) = (std::byte)0xEB;
    //    VirtualProtect((void*)pSS_DISCONNECT_PATCH_TWO, 4, protect1, &protect1);
    //}
#endif



}




// https://github.com/perilouswithadollarsign/cstrike15_src/blob/master/game/client/c_baseanimating.cpp#L5090
// https://www.unknowncheats.me/forum/counterstrike-global-offensive/455619-drawmodelexecute-chams-hidden.html



typedef void*(__cdecl* SteamNetSocketsMemcpyFunc_t)(void*, void*, size_t);
inline SteamNetSocketsMemcpyFunc_t oSteamNetSocketsMemcpy;
void* __cdecl hk_SteamNetSockets_MemCpy_HDRHeader(void* pData, void* pSrc, size_t nSize)
{
    void* ret = oSteamNetSocketsMemcpy(pData, pSrc, nSize);

#if 1
    if (GetAsyncKeyState(VK_NUMLOCK))
    {
        //int nUpper3Bits = (0xFF >> 8);
        // The values 5 and 6 are reserved and shouldn't be needed due to the MTU we support
        std::memset(pData, (byte)(0x20 | (0 & 0x1f)), nSize);
        //*((char*)pData) |= 0x40;
    }
#endif
    return ret;
}




EncodedSegment* g_pSegment = NULL;
void* g_pJumpBackAddr = NULL;
void SegSizeHook()
{
    if (GetAsyncKeyState(VK_NUMLOCK))
    {
        g_pSegment->m_cbSegSize = 0;
        g_pSegment->m_pMsg->m_cbSize = 0;
    }
}


__declspec(noinline) void hk_SegSize()
{
    _asm
    {
        push ebp
        push esi
        push ecx
        push edx
        push eax
        mov g_pSegment, ebp
        call SegSizeHook
        pop eax
        pop edx
        pop ecx
        pop esi
        pop ebp
        add esp, 0x0C
        push eax
        push ebx
        push esi
        call hk_SteamNetSockets_MemCpy_HDRHeader
        jmp g_pJumpBackAddr
    }
}
// 03 C6 89 75 F0
/// Describe the state of a connection.



typedef void(__thiscall* UpdateConnectionInfoFunc_t)(void*);
UpdateConnectionInfoFunc_t oUpdateConnectionInfo = NULL;
void __fastcall hk_SteamNetworkingSockets_UpdateConnectionInfo(void* ecx, void* edx)
{
    // E8 ? ? ? ? 85 FF 74 10
    oUpdateConnectionInfo(ecx);
    SteamNetConnectionInfo_t* pConnInfo = reinterpret_cast<SteamNetConnectionInfo_t*>((char*)ecx + 440);
    pConnInfo->m_eState = k_ESteamNetworkingConnectionState_Connected;
    return;
}


LPVOID cbDesiredSegSizeJmpBack = NULL;
__declspec(noinline) void hk_ModifycbDesiredSegSize(void)
{
    _asm {
        push eax
        push esi
        push ecx
        push ebp
        push edx
        push VK_NUMLOCK
        call GetAsyncKeyState
        cmp eax, 1
        jne no_modify
        mov edx, 0
        pop edx
        jmp return_and_exit
no_modify:
        pop edx
        mov edx, [edx + 4]
        sub edx, ecx
return_and_exit:
        pop ebp
        pop ecx
        pop esi
        pop eax
        jmp cbDesiredSegSizeJmpBack
    }
}




DEBUGNOINLINE void INTERNAL_HOOKS::InitializeHooks()
{
    SetupSteamLibrary();



    m_pHooker = new MinHookWrapper();
    //m_pHooker->Initialize(); // We Call Initalize in constructor
#if 1
    clientMode = new ClassHook(m_pHooker, g_pInterfaces->m_pClientMode);
    client = new ClassHook(m_pHooker, g_pInterfaces->m_pClient);
    studioRender = new ClassHook(m_pHooker, g_pInterfaces->m_pStudioRender);
    surface = new ClassHook(m_pHooker, g_pInterfaces->m_pSurface);
    engine = new ClassHook(m_pHooker, g_pInterfaces->m_pEngine);
    entityListener = new ClassHook(m_pHooker, *((void**)g_pMemory->GetAddress(__ICLIENTENTITYLIST)));
    clientState = new ClassHook(m_pHooker, **reinterpret_cast<char***>((*reinterpret_cast<uintptr_t**>(g_pInterfaces->m_pEngine))[12] + 0x10) + 8);
    clientMode->Init();
    studioRender->Init();
    engine->Init();
    entityListener->Init();
    surface->Init();
    client->Init();
    clientState->Init();

    clientMode->HookAtIndex(24, &hk_CreateMove, DEBUGSTR("ClientMode::CreateMove"));
    //studioRender->HookAtIndex(8, &hk_BeginFrame, DEBUGSTR("IStudioRender::BeginFrame"));
    surface->HookAtIndex(67, &hk_LockCursor, DEBUGSTR("IStudioRender::LockCursor"));
    engine->HookAtIndex(93, &hk_IsHLTV, DEBUGSTR("IVEngineClient::IsHltv"));
    entityListener->HookAtIndex(0, &hk_OnEntityCreated, DEBUGSTR("IEntityListener::OnEntityCreated"));
    entityListener->HookAtIndex(1, &hk_OnEntityDeleted, DEBUGSTR("IEntityListener::OnEntityCreated"));
    client->HookAtIndex(37, &hk_CHLClient_FrameStageNotify, DEBUGSTR("FrameStageNotify"));
    clientState->HookAtIndex(63, &hk_SetReservationCookie, DEBUGSTR("CCClientState::SetReservationCookie"));

    oIsHLTV = (IsHLTVFunc_t)engine->GetOriginalAtIndex(93);
   
    m_pHooker->FunctionHook(g_pMemory->GetAddress(__HOSTRUNFRAMEINPUT), &hk_HostRunFrameInput, (void**)&oHostRunFrameInput, DEBUGSTR("HostRunFrameInput"));
    m_pHooker->FunctionHook(g_pMemory->GetAddress(__CBASEPLAYER_ONDATACHANGED), &hk_CBasePlayer_OnDataChanged, (void**)&oCBasePlayerOnDataChanged, DEBUGSTR("CBasePlayer::OnDataChanged"));
    m_pHooker->FunctionHook(g_pMemory->GetAddress(__CNETCHAN_PROCESSPACKET), &hk_CNetChan_ProcessPacket, (void**)&oCNetChanProcessPacket, DEBUGSTR("CNetChan::ProcessPacket"));
    m_pHooker->FunctionHook(g_pMemory->GetAddress(__DOEXTRABONEPROCESSING), &hk_C_CSPlayer_DoExtraBoneProcessing, (void**)&oDoExtraBoneProcessing, DEBUGSTR("C_CSPlayer::DoExtraBoneProcessing"));
    m_pHooker->FunctionHook(g_pMemory->GetAddress(__SETUPBONES), &hk_BaseAnimating_SetupBones, (void**)&oSetupBones, DEBUGSTR("C_BaseAnimating::SetupBones"));
    m_pHooker->FunctionHook(g_pMemory->GetAddress(__HOSTRUNFRAMERENDER), &_Host_RunFrame_Render, (void**)&g_poHostRunFrameRender, DEBUGSTR("HOSTRUNFRAMERENDER"));
    m_pHooker->FunctionHook(g_pMemory->GetAddress(__CSHADERSYSTEM_DRAWELEMENTS), &hk_CShaderSystem_DrawElements, (void**)&oCShaderSystemDrawElements, DEBUGSTR("CShaderSystem::DrawElements"));
    m_pHooker->FunctionHook(g_pMemory->GetAddress(__CSHADERSYSTEM_PREPFORSHADERDRAW), &hk_CShaderSystem_PrepForShaderDraw, (void**)&oCShaderSystemPrepForShaderDraw, DEBUGSTR("CShaderSystem::PrepForShaderDraw"));
    m_pHooker->FunctionHook(g_pMemory->GetAddress(__SHOULDSKIPANIMATIONFRAME), &hk_CBaseAnimating_ShouldSkipAnimationFrame, (void**)&oShouldSkipAnimationFrame, DEBUGSTR("C_BaseAnimating::ShouldSkipAnimationFrame"));
    m_pHooker->FunctionHook(g_pMemory->GetAddress(__UNKNOWNRENDERVIEWFUNCTION2), &hk_UnknownViewRenderFunction, (void**)&oUnknownViewRenderFunction, DEBUGSTR("UnknownViewRenderFunc"));
    m_pHooker->FunctionHook(g_pMemory->GetAddress(__SNP_SENDMESSAGE), &hk_SNP_SendMessage, (void**)&oSNP_SendMessage, DEBUGSTR("CSteamNetworkConnectionBase::SNP_SendMessage"));
    m_pHooker->FunctionHook(g_pMemory->GetAddress(__CSTEAMSOCKETMGR_ONP2PSESSIONREQUEST), &hk_CSteamSocketMgr_OnP2PSessionRequest, (void**)&oCSteamSocketMgr_OnP2PSessionRequest, DEBUGSTR("CSteamSocketMgr::OnP2PSessionRequest"));
    m_pHooker->FunctionHook(g_pMemory->GetAddress(__PROCESSCONNECTIONLESSPACKET), &hk_ProcessConnectionless, (void**)&oProcessConnectionless, DEBUGSTR("CBaseClientState::ProcessConnectionlessPacket"));
   // 
    //__CSTEAMSOCKETMGR_ONP2PSESSIONREQUEST

    //m_pHooker->FunctionHook(g_pMemory->GetAddress(__SNP_GETNEXTTHINKTIME), &hk_SNP_GetNextThinkTime, (void**)&oSNP_GetNextThinkTime, DEBUGSTR("CSteamNetworkConnectionBase::SNP_GetNextThinkTime"));  
    //m_pHooker->FunctionHook(g_pMemory->GetAddress(__SENDSUBCHANNEL), &hk_SendSubChannelData, (void**)&oSendSubChannelData, DEBUGSTR("CNetChan::SendSubChannelData"));

    MemoryTools::g_bResolveRelative = false;
    BYTE* SteamNetSocketsMemcpyCallAddr = (BYTE*)((char*)MemoryTools::FindPattern("steamnetworkingsockets", XorStr("\x56\xE8????\x03\x73\x10"), DEBUGSTR("Memcpy Of HDR")) + 1);
    oSteamNetSocketsMemcpy = MemoryTools::_relativeToAbsolute<SteamNetSocketsMemcpyFunc_t>((uintptr_t)((char*)SteamNetSocketsMemcpyCallAddr + 1));
    if (!SteamNetSocketsMemcpyCallAddr)
        __debugbreak();
    CON(hexStr(SteamNetSocketsMemcpyCallAddr, 20).c_str());
    //MemoryTools::PlaceCALL(SteamNetSocketsMemcpyCallAddr, (DWORD)&hk_SteamNetSockets_MemCpy_HDRHeader, 5);
    CON(hexStr(SteamNetSocketsMemcpyCallAddr, 20).c_str());
    CON(hexStr((BYTE*)&oSteamNetSocketsMemcpy, 4).c_str());
    MemoryTools::g_bResolveRelative = true;

    // BYTE* SteamNetSocketsSegmentAddr = (BYTE*)((char*)MemoryTools::FindPattern("steamnetworkingsockets", XorStr("\x83\xC4\x0C\x50\x53\x56"), DEBUGSTR("GrabSegSize")));
    // MemoryTools::PlaceJMP(SteamNetSocketsSegmentAddr, (DWORD)&hk_SegSize, 11);
    // g_pJumpBackAddr = (char*)SteamNetSocketsSegmentAddr + 12;

    // E8 ? ? ? ? 85 FF 74 10
    LPVOID* UpdateConnectionInfoAffer  = MemoryTools::FindPattern<LPVOID>(__STEAMNETWORKING, XorStr("\xE8????\x85\xFF\x74\x10"), 1, DEBUGSTR("Update Connection Info"));
    m_pHooker->FunctionHook(UpdateConnectionInfoAffer, &hk_SteamNetworkingSockets_UpdateConnectionInfo, (void**)&oUpdateConnectionInfo, DEBUGSTR("Update Connection Info"));



    // BYTE* pCbDesiredSegSizeAddr = (BYTE*)((char*)MemoryTools::FindPattern("steamnetworkingsockets", XorStr("\x8B\x52\x04\x2B\xD1"), DEBUGSTR("cbDesiredSegSize")));
    // cbDesiredSegSizeJmpBack = pCbDesiredSegSizeAddr + 5;
    // MemoryTools::PlaceJMP(pCbDesiredSegSizeAddr, (DWORD)hk_ModifycbDesiredSegSize, 5);


    // Hook SendConnectPacket
    MemoryTools::g_bResolveRelative = false;
    g_pSendConnectPacketJumpBack = (std::uintptr_t)MemoryTools::FindPattern(__ENGINE, XorStr("\x8B\x87????\x85\xC0\x8B\x0D????"), 0, DEBUGSTR("m_pSendConnectPacketJumpOut"));
    auto jmpOut = (char*)MemoryTools::FindPattern(__ENGINE, XorStr("\xE8????\x51\x6A\x08\x6A\x6B\x8D\x4C\x24\x18"), 0, DEBUGSTR("JumpOut"));    
    //MemoryTools::PlaceJMP((BYTE*)jmpOut, (DWORD)&OnSendConnectPacket, 5);
    MemoryTools::g_bResolveRelative = true;

   // 
    //void* SendConnectAddr = (void*)MemoryTools::FindPattern(__ENGINE, XorStr("\x55\x8B\xEC\x83\xE4\xF8\x81\xEC????\x53\x56\x57\x68????\x8B\xF9\xFF\x15????\x8B\x5D\x08"), 0, DEBUGSTR("CBaseClientState::SendConnectPacket"));
    //m_pHooker->FunctionHook(SendConnectAddr, &hk_SendConnectPacket, (void**)&oSendConnectPacket, DEBUGSTR("CBaseClientState::SendConnectPacket"));

    void* ConnectInternalAddr = (void*)MemoryTools::FindPattern(__ENGINE, XorStr("\x55\x8B\xEC\x83\xE4\xF8\x83\xEC\x4C\x53\x8B\xD9\x8B\x0D????"), 0, DEBUGSTR("CBaseClientState::ConnectInternal"));
    m_pHooker->FunctionHook(ConnectInternalAddr, &hk_CBaseClientState_ConnectInternal, (void**)&oConnectInternal, DEBUGSTR("CBaseClientState::ConnectInternal"));


    DWORD server_received_signonstate = (DWORD)MemoryTools::FindPattern(__ENGINE, "\x55\x8B\xEC\x8B\x45\x08\x8B\x11\x50\xFF\x70\x0C\xFF\x70\x08\x8B\x82????\xFF\xD0\x5D\xC2\x04\x00");
    if (server_received_signonstate)
        m_pHooker->FunctionHook((LPVOID)server_received_signonstate, &ReceivedServerSignonState, (LPVOID*)&oReceivedServerSignonState, XorStr("ReceivedServerSignonState"));

    RunBytePatches();
    SetupKeyValues();
    
    
    g_pChamRenderer = new ChamRenderer();


    oPresent = **reinterpret_cast<decltype(oPresent)**>(g_pMemory->GetAddress(__PRESENT));
    **reinterpret_cast<decltype(present)***>(g_pMemory->GetAddress(__PRESENT)) = present;
    oReset = **reinterpret_cast<decltype(oReset)**>(g_pMemory->GetAddress(__RESET));
    **reinterpret_cast<decltype(reset)***>(g_pMemory->GetAddress(__RESET)) = reset;
#endif


    DWORD CL_MoveSig = (DWORD)g_pMemory->GetAddress(__CLMOVE);//(DWORD)MemoryTools::FindPattern(XorStr("engine"), XorStr("\x55\x8B\xEC\x81\xEC????\x53\x56\x57\x8B\x3D????\x8A"));
    oCL_Move = reinterpret_cast<CL_MoveFunc>(CL_MoveSig + 9);
    MemoryTools::PlaceJMP((BYTE*)(CL_MoveSig), (DWORD)&hk_CL_Move, 9);


    m_pHooker->InitializeHooks();

    InitalizeOurConvars();
   InitalizeOurConCommands();

    _beginthreadex(0, 0, FreezerThread, 0, NULL, 0);
    _beginthreadex(0, 0, BlockThread, 0, NULL, 0);

    
}

static DWORD WINAPI CleanUp(HMODULE wmodule) noexcept {
    //_CRT_INIT(wmodule, DLL_PROCESS_DETACH, nullptr);
    FreeLibraryAndExitThread(wmodule, 0);
}

void INTERNAL_HOOKS::Disable() {

    m_pHooker->DisableHooks();
#ifdef IMGUI_MENU
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
#endif
    SetWindowLongPtrW(window, GWLP_WNDPROC, LONG_PTR(originalWndProc));
    **reinterpret_cast<void***>(g_pMemory->GetAddress(__PRESENT)) = oPresent;
    **reinterpret_cast<void***>(g_pMemory->GetAddress(__RESET)) = oReset;
    if (HANDLE thread = CreateThread(nullptr, 0, LPTHREAD_START_ROUTINE(CleanUp), wmodule, 0, nullptr))
        CloseHandle(thread);
}



void RunBytePatches()
{
    // Thank you valve compiler for making every single net_ws.cpp function a bitch tit and a quarter to hook
    // Get Address of value in memory
    std::byte* pAddr = (std::byte*)(g_pMemory->GetAddress(__NETCREATECHANNEL));
    VCON(XorStr("NET_CreateChannel Hook - { %s }\n"), hexStr((BYTE*)pAddr - 5, 20).c_str());
    Net_CreateNetChannel_ValToIncrement = *(DWORD**)(pAddr + 2);
    DWORD protect = 0;
    VirtualProtect(pAddr, 10, PAGE_EXECUTE_READWRITE, &protect);
    for (int i = 0; i < 4; i++) // move the { mov  eax, [esp+60h+var_58] } up 1 instruction
        *(pAddr + i) = *(pAddr + 6 + i);
    VirtualProtect(pAddr, 10, protect, &protect);
    // Write our jump
    MemoryTools::PlaceJMP((BYTE*)(pAddr + 0x4), (DWORD)&hk_NET_CreateNetChannel, 6);
    oNET_CreateNetChannel = pAddr + 10; // Finally Calculate the address to jump back too
    VCON(XorStr("NET_CreateChannel Hook - { %s }\n"), hexStr((BYTE*)pAddr - 5, 20).c_str());

    CON(XorStr("Patching Trusted Mode and OBS Block\n"));
    TrustedModeBypass();

    CON(XorStr("Patching FPS Max Clamp\n"));
    uintptr_t FPSMaxClamp = MemoryTools::FindPattern<std::uintptr_t>(XorStr("engine"), XorStr("\x0F\x2F\x45\xFC\x76\x75")) + 4;
    if (FPSMaxClamp)
    {
        VirtualProtect((void*)FPSMaxClamp, 4, PAGE_EXECUTE_READWRITE, &protect);
        *(std::byte*)(FPSMaxClamp) = (std::byte)0xEB;
        VirtualProtect((void*)FPSMaxClamp, 4, protect, &protect);
    }


    //MemoryTools::g_bResolveRelative = false;
    //char* pReadMessageType = (char*)MemoryTools::FindPattern(__ENGINE, XorStr("\x8B\xCE\xE8????\x8B\x97????"), 0, 0) + 2; // 4
    //MemoryTools::PlaceJMP((BYTE*)pReadMessageType, (DWORD)&hk_CNetChann_ProcessMessage, 5);
    //MemoryTools::g_bResolveRelative = true;
    //E8 ? ? ? ? 8B 16 8B CE 8A D8 FF 92 ? ? ? ? 84
    // 13
#if 0
    char* nop_addr = pReadMessageType + 12;
    VirtualProtect((void*)nop_addr, 4, PAGE_EXECUTE_READWRITE, &protect);
    for (int i = 0; i < 3; i++)
    {
        *(nop_addr + i) = 0x90;
    } 
    VirtualProtect((void*)nop_addr, 4, protect, &protect);
#endif





}








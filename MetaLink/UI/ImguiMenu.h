#pragma once 
#include "../PREPROCESSOR.h"

#ifdef IMGUI_MENU
#include "../ThirdParty/imgui-master/imgui.h"
#include "../ThirdParty/imgui-master/imgui_internal.h"
#include "../ThirdParty/imgui-master/backends/imgui_impl_win32.h"
#include "../ThirdParty/imgui-master/backends/imgui_impl_dx9.h"

#include <d3d9helper.h>

#include "C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\d3dx9.h"
#pragma comment(lib, "C:\\Program Files (x86)\\Microsoft DirectX SDK (June 2010)\\lib\\x86\\d3dx9.lib")

inline LPDIRECT3DDEVICE9 g_pd3dDevice;
namespace ImGuiMenu {
	void Menu();
	void Init();
	void __stdcall hkLockCursor();
	inline bool g_bIsMenuOpen{ false };
}

#endif
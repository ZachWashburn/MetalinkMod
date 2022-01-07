#pragma once

#ifndef INTERNAL_HOOKS_H
#define INTERNAL_HOOKS_H
#include "BaseIncludes.h"
#include "MinHookWrap.h"
class INTERNAL_HOOKS {
public:
	INTERNAL_HOOKS() { Initialize(); }
	~INTERNAL_HOOKS() {
		for (void* address : m_vAddressesToFree) {
			if (address)
				delete address;
		}
	}
	DEBUGNOINLINE void Initialize();
	DEBUGNOINLINE void InitializeHooks();
	void EnableHooks() { m_pHooker->InitializeHooks(); }
	//void PostInitHooks() { m_pHooker->InitializeHooks(); }
	void Disable();
	HookType* GetHooker() { return m_pHooker; };
	ClassHook* networkChannel; 
	ClassHook* engine;
	ClassHook* clientMode;
	ClassHook* client;
	ClassHook* studioRender;
	ClassHook* surface;
	ClassHook* modelRender;
	ClassHook* bspQuery;
	ClassHook* renderView;
	ClassHook* viewRender;
	ClassHook* prediction;
	ClassHook* svCheats;
	ClassHook* entityListener;
	ClassHook* clientState;
	WNDPROC originalWndProc;
	HMODULE wmodule;
	HWND window;
private:
	HookType* m_pHooker;
	std::vector<void*> m_vAddressesToFree;
};

inline std::unique_ptr<INTERNAL_HOOKS> g_pHooks;

#endif
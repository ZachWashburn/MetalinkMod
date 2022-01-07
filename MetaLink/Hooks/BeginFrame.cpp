#include "../BaseIncludes.h"
#include "SDK.h"
#include "BeginFrame.h"
#include "../Hooks.h"
#include "../Hacks/DebugEsp.h"

typedef void(__thiscall* BeginFrameFunc_t)(IStudioRender*);
void __fastcall hk_BeginFrame(IStudioRender* _this, void*)
{
	SCOPESECTION(__SECTION_BEGINFRAME);
	static BeginFrameFunc_t pOriginal{ reinterpret_cast<BeginFrameFunc_t>(g_pHooks->studioRender->GetOriginalAtIndex(8)) };

#ifndef _LIVE
	if(g_pInterfaces->m_pEngine->IsInGame() && g_pInterfaces->m_pEngine->IsConnected())
		DebugEsp::RunFrame();
#endif


	pOriginal(_this);
}





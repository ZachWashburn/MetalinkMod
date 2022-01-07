#include "../BaseIncludes.h"
#include "ChamsHooks.h"
#include "SDK.h"
#include "../Memory.h"
#include "../Hacks/Chams.h"



int __fastcall hk_UnknownViewRenderFunction(void* _this, void* edx, int a2)
{
	g_pChamRenderer->OnSceneEnd();
	return oUnknownViewRenderFunction(_this, a2);
}
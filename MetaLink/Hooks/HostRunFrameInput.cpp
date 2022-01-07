#include "HostRunFrameInput.h"
#include "../FunctionTypeDeclares.h"
#include "../Memory.h"
#include "Interfaces.h"
#include "SDK.h"

//#define REBUILD_HOST_RUN_FRAME_INPUT



__forceinline void __cdecl HostRunFrameInput(float flAccumulatedExtraSamples, bool bFinalTick)
{
	static double pHostTimeStart{ 0 };
	static CL_MoveFunc_t CL_Move{ *reinterpret_cast<CL_MoveFunc_t>(g_pMemory->GetAddress(__CLMOVE)) };
	static CBufExecuteFunc_t CBuf_Execute{ reinterpret_cast<CBufExecuteFunc_t>(g_pMemory->GetAddress(__HOSTRUNFRAMEINPUT) + 0x127) };
	static double** pHostInputStartTime{ MemoryTools::FindPattern<double*>(__ENGINE, XorStr("\xDD\x1D????\x74\x2D")) + 2  };
	static double** pTime{ MemoryTools::FindPattern<double*>(__ENGINE, XorStr("\xF2\x0F\x11\x05????\xC6\x83?????")) + 4 };
	static Plat_FloatTimeFunc_t Plat_FloatTime{ reinterpret_cast<Plat_FloatTimeFunc_t>(GetProcAddress(GetModuleHandleA(XorStr("tier0")), XorStr("Plat_FloatTime"))) };
	static double* pFrameTime{ (*(double**)((char*)g_pMemory->GetAddress(__HOSTRUNFRAMEINPUT) + 0x121)) };
	__asm
	{
		call Plat_FloatTime
		fstp pHostInputStartTime
	}
	float flFrameTime = *pFrameTime;
	__asm
	{
		call Plat_FloatTime
		fstp flFrameTime
		call Plat_FloatTime
		fsub flFrameTime
	}
	*pFrameTime = flFrameTime;
	if (g_pInterfaces->m_pClient)
	{
		//static ProcessInputFunc_t HudProcessInput{ reinterpret_cast<ProcessInputFunc_t>((*reinterpret_cast<void***>(g_pInterfaces->m_pClient))[10])};
		//HudProcessInput(true);
		g_pInterfaces->m_pClient->HudProcessInput(true);
	}
	CBuf_Execute();
	__asm movss xmm0, flAccumulatedExtraSamples
	__asm mov cl, bFinalTick
	__asm call CL_Move




}

void __cdecl hk_HostRunFrameInput(void*, void*, float flAccumulatedExtraSamples, bool bFinalTick)
{
	SCOPESECTION(__SECTION_FRAMEINPUT);
#ifdef REBUILD_HOST_RUN_FRAME_INPUT
	HostRunFrameInput(flAccumulatedExtraSamples, bFinalTick);
#else

	//DEBUG_OUT_STR_VAR("Save Dir : %s", g_pInterfaces->m_pEngine->GetSaveDirName());

	static HostRunFrameInputFunc_t pOriginal{ reinterpret_cast<HostRunFrameInputFunc_t>(oHostRunFrameInput) };

	pOriginal(flAccumulatedExtraSamples, bFinalTick);
#endif
}
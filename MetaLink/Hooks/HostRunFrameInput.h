#pragma once
#include "../BaseIncludes.h"
inline LPVOID* oHostRunFrameInput{ NULL };
__forceinline void __cdecl HostRunFrameInput(float flAccumulatedExtraSamples, bool bFinalTick);
void __cdecl hk_HostRunFrameInput(void*, void*, float flAccumulatedExtraSamples, bool bFinalTick);

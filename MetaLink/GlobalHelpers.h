#pragma once
#include "FunctionTypeDeclares.h"
#include "tier0/platform.h"
__forceinline double GetFloatTime()
{
	return ((Plat_FloatTimeFunc_t)Plat_FloatTime)();
}
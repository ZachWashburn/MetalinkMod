#include "RebuiltWindowsAPI.h"





FARPROC WINAPI Rebuild_GetProcAddress(HMODULE hModule, LPCSTR lpProcName)
{
	if (!hModule || !lpProcName)
		return nullptr;









}
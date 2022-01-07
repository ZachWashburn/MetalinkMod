#pragma once 
#include "../PREPROCESSOR.h"
#include <Windows.h>
#include <malloc.h>

#include "../ThirdParty/Custom-GetProcAddress-and-GetModuleHandle/CustomWinApi.h"
// Ran Out Of Time, using third party library. TODO : Rebuild Myself!!!!!!!!
//FARPROC WINAPI Rebuild_GetProcAddress(HMODULE hModule, LPCSTR lpProcName);
//HMODULE WINAPI Rebuild_GetModuleHandleA(LPCSTR  lpModuleName);


#ifdef OVERRIDE_WINDOWS_API
//#define GetProcAddress Rebuild_GetProcAddress
//#define GetModuleHandleA Rebuild_GetModuleHandleA

#define GetProcAddress GetExportAddress
#define GetModuleHandleA GetModuleA
#define GetModuleHandleW GetModuleW
#endif
#pragma once
#include "../BaseIncludes.h"



typedef enum _HARDERROR_RESPONSE_OPTION {
	OptionAbortRetryIgnore,
	OptionOk,
	OptionOkCancel,
	OptionRetryCancel,
	OptionYesNo,
	OptionYesNoCancel,
	OptionShutdownSystem
} HARDERROR_RESPONSE_OPTION, * PHARDERROR_RESPONSE_OPTION;



typedef NTSTATUS(NTAPI* pdef_NtRaiseHardError)(NTSTATUS ErrorStatus, ULONG NumberOfParameters, ULONG UnicodeStringParameterMask OPTIONAL, PULONG_PTR Parameters, ULONG ResponseOption, PULONG Response);
typedef NTSTATUS(NTAPI* pdef_RtlAdjustPrivilege)(ULONG Privilege, BOOLEAN Enable, BOOLEAN CurrentThread, PBOOLEAN Enabled);

/* 
	We want the loader to stay open and take care of most of this, but we also 100% need to keep track of this locally aswell.
	We need to keep tabs with the loader (ensure its open and exists), keep tabs with the server (make sure ET can phone home),
	log info about debuggers etc, check to see if on a steam server or community server, and see if there are large gaps in network
	connectivity.
*/


enum ViolationDefcon {
	NONE,
	PROGRAMEXIT,
	LOW, // Probably our error or some sorta enviroment bug, just clear all and exit
	MEDIUM, // Lets Clear Everything and enforce temp wait time
	HIGH, // Clear Everything, user cant inject until mod approves them as okay
	SEVERE, // Lets Crash the PC, user is banned
};

class Security {
	Security() {};
	__forceinline void OnDatagram();
	__forceinline void OnLevelInit();
	__forceinline void OnUninject();
};

bool EnablePriviledge(HANDLE hAccessToken, LPCSTR szPrivledge);
bool PreSecurityThreadStart(HANDLE hHandle);
DWORD WINAPI SecurityMainThread(void* hHandle);
__forceinline void ThrowBSOD();


inline bool g_bViolationDetected{ false };
inline ViolationDefcon g_nViolationLevel{ NONE };
inline Security* g_pSecurity;


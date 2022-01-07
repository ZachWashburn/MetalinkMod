#include "Security.h"



__forceinline void ThrowBSOD() { // Pasted Ass Function, lets find a better way
	BOOLEAN bEnabled;
	ULONG uResp;
	LPVOID lpFuncAddress = GetProcAddress(LoadLibraryA(XorStr("ntdll.dll")), XorStr("RtlAdjustPrivilege"));
	LPVOID lpFuncAddress2 = GetProcAddress(GetModuleHandle(XorStr("ntdll.dll")), XorStr("NtRaiseHardError"));
	pdef_RtlAdjustPrivilege NtCall = (pdef_RtlAdjustPrivilege)lpFuncAddress;
	pdef_NtRaiseHardError NtCall2 = (pdef_NtRaiseHardError)lpFuncAddress2;
	NTSTATUS NtRet = NtCall(19, TRUE, FALSE, &bEnabled);
	NtCall2(STATUS_FLOAT_MULTIPLE_FAULTS, 0, 0, 0, OptionShutdownSystem, &uResp);
}



bool EnablePriviledge(HANDLE hAccessToken, LPCSTR szPrivledge) {
	TOKEN_PRIVILEGES tp;
	LUID PrivilegeLUID;
	bool bRes = LookupPrivilegeValue(NULL, szPrivledge, &PrivilegeLUID);
	if (!bRes)
		return true;

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = PrivilegeLUID;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;


	if (!AdjustTokenPrivileges(hAccessToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES)NULL, (PDWORD)NULL))
		return true;

	return false;
}


bool PreSecurityThreadStart(HANDLE hHandle) {
	bool bRet = false;
	HANDLE ProcessToken = 0;
	LPCSTR RequiredTokens[] = { SE_DEBUG_NAME , SE_LOCK_MEMORY_NAME , SE_LOAD_DRIVER_NAME , SE_PROF_SINGLE_PROCESS_NAME,SE_SECURITY_NAME,SE_SHUTDOWN_NAME,SE_SYSTEM_PROFILE_NAME, SE_SYSTEM_ENVIRONMENT_NAME};
	if (!OpenProcessToken(hHandle, TOKEN_ALL_ACCESS, &ProcessToken))
		return true;

	for (int i = 0; i < (sizeof(RequiredTokens) / 4); i++) {
		bRet = bRet || EnablePriviledge(ProcessToken, RequiredTokens[i]);
	};

	return bRet;
}

DWORD WINAPI SecurityMainThread(void* hHandle) {
	bool bDebuggerAttached = false;
	while ((g_nViolationLevel == NONE) && !g_bViolationDetected) {
		CheckRemoteDebuggerPresent(hHandle, (PBOOL)&bDebuggerAttached);
		bool bDebugger = IsDebuggerPresent() || bDebuggerAttached;
		if (bDebugger) {
			g_bViolationDetected = true;
			g_nViolationLevel = SEVERE;
			break;
		}
	}


	if (g_nViolationLevel == SEVERE) {
#ifdef _DEBUG
		OutputDebugStringA("SEVERE INFRACTION");

#endif

#ifdef _LIVE // Force PC Crash after notifying Server
		ThrowBSOD();
		return 1; // Like it ever makes it here
#endif
	}


	if (g_nViolationLevel == HIGH) {
#ifdef _DEBUG
		OutputDebugStringA("HIGH INFRACTION");
#endif

#ifdef _LIVE // Force Game Crash after notifying Server
		QuickCrash;
		return 1;
#endif
	}
	return 0;


}
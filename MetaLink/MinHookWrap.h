#pragma once

#include "ThirdParty/MinHook/MinHook.h"
#include "ThirdParty/MinHook/trampoline.h"
#include "HookType.h"


class MinHookWrapper : public HookType {
public:
	MinHookWrapper() { Initialize(); }
	bool FunctionHook(void* pOrgFunction, void* pHkFunctionAddress, void** ppoAddressReturn = nullptr, const char* szFunctionName = nullptr) {
		LPVOID pOriginalFunctionAddress;
		if (szFunctionName) { DEBUG_OUT_STR_VAR(("Attempting to Hook Function %s ..."), szFunctionName); }
		int RET = MH_CreateHook(pOrgFunction, pHkFunctionAddress,reinterpret_cast<LPVOID*>(&pOriginalFunctionAddress));
		if (RET != MH_OK) {
			if (szFunctionName) { DEBUG_OUT_STR_VAR(("FAILED\n --- Error On Creation Of %s Hook : %d --- \n"), szFunctionName, RET); }
			if (MH_DisableHook(pOrgFunction) != MH_OK)
			{
				if (szFunctionName) { DEBUG_OUT_STR_VAR((" --- Error On Disabling Of %s Hook --- \n"), szFunctionName); }
				return false;
			}
			else {
				if (szFunctionName) { DEBUG_OUT_STR_VAR(("Successfully Disabled %s Hook \n"), szFunctionName); }
				return false;
			}
		}
		else {
			if (szFunctionName) { DEBUG_OUT_STR_VAR("Ok\n"); }
		}
		if (ppoAddressReturn)
			*ppoAddressReturn = pOriginalFunctionAddress;
		return true;
	}

	bool Initialize() {
		MH_STATUS mhStatus = MH_Initialize();
#ifdef _DEBUG
		DEBUG_OUT_STR_VAR("MH_Status : %d\n", mhStatus);
#endif
		_ASSERT(mhStatus == MH_OK);

		return true;
	}

	bool InitializeHooks() {
		if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK) {
			X_DEBUG_OUT_STR("MINHOOK FAILED TO ENABLE HOOKS!\n")
			return false;
		}
		return true;		
	}

	bool DisableHooks() {
		if (MH_DisableHook(MH_ALL_HOOKS) != MH_OK) {
			X_DEBUG_OUT_STR("Unable To Disable Hooks (MINHOOK)");
			return false;
		}

		if (MH_Uninitialize() != MH_OK) {
			X_DEBUG_OUT_STR("Unable To Un-Initialize Hooks (MINHOOK)");
			return false;
		}

		return true;
	}
private:
};










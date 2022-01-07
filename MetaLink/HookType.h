#pragma once
#include "BaseIncludes.h"
namespace HookHelpers {
	inline auto calculateVmtLength(uintptr_t* vmt) noexcept
	{
		unsigned int length = 0;
		MEMORY_BASIC_INFORMATION memoryInfo;
		while (VirtualQuery(
			LPCVOID(vmt[length]),
			&memoryInfo,
			sizeof(memoryInfo)) && memoryInfo.Protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)) {
			length++;
		}
		return length;
	}
}
class HookType {
public:
	virtual bool FunctionHook(void* pOrgFunction, void* pHkFunctionAddress, void** ppoAddressReturn = nullptr, const char* szFunctionName = nullptr) = 0;
	virtual bool Initialize() = 0;
	virtual bool InitializeHooks() = 0;
	virtual bool DisableHooks() = 0;
private:
};
class ClassHook {
public:
	ClassHook() {};
	ClassHook(HookType* pHooker) : m_pHooker(pHooker) {};
	ClassHook(HookType* pHooker, void* pInBase) : m_pHooker(pHooker), m_pBaseAddress(pInBase) {};
	~ClassHook() { if (m_pOriginals) { free(m_pOriginals); } }
	void Init() {
		if (!m_pBaseAddress)
			X_DEBUG_OUT_STR("No Base Addresses Provided During a ClassHook::Init() Call!")

		int vmt_length = HookHelpers::calculateVmtLength(*reinterpret_cast<uintptr_t**>(m_pBaseAddress));
		m_pOriginals = (uintptr_t*)calloc(1,sizeof(uintptr_t)*vmt_length);
	}

	void Init(HookType* pHooker, void* pInBase) {
		m_pHooker = pHooker;
		m_pBaseAddress = pInBase;
		Init();
	}

	void SetBaseAddress(void* pInBase) { m_pBaseAddress = pInBase; }
	bool HookAtIndex(unsigned int nIndex, void* pHkFunctionAddress, const char* szFunctionName = nullptr) {
		if (!m_pBaseAddress || !pHkFunctionAddress) {
			if (!m_pBaseAddress)
			{
				DEBUG_OUT_EXIT_ERROR(XorStr("ClassHook Called Without Valid BaseAddress\n"))
			}
			if (!pHkFunctionAddress)
			{
				DEBUG_OUT_EXIT_ERROR(XorStr("ClassHook Called With Invalid FunctionAddress\n"))
			}
		}	
		if (szFunctionName) { DEBUG_OUT_STR_VAR("ClassHook::HookAtIndex Attempting to Initialize Function %s\n", szFunctionName) }
		if (!m_pHooker->FunctionHook((*reinterpret_cast<void***>(m_pBaseAddress))[nIndex], pHkFunctionAddress, (void**)&(m_pOriginals[nIndex]), szFunctionName)) {
			if (szFunctionName) DEBUG_OUT_STR_VAR("ClassHook::HookAtIndex Failed To Initialize %s\n", szFunctionName)
			return false;
		}
		if (szFunctionName) { DEBUG_OUT_STR_VAR("ClassHook::HookAtIndex Successfully Hooked Function %s\n", szFunctionName); }
		return true;
	};
	void* GetOriginalAtIndex(int nIndex) { return (void*)m_pOriginals[nIndex]; }
	void* GetBase() { return m_pBaseAddress; }
private:
	uintptr_t* m_pOriginals = nullptr;
	void* m_pBaseAddress = nullptr;
	HookType* m_pHooker = nullptr;
};
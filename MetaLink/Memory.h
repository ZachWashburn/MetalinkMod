#pragma once
#include "BaseIncludes.h"

#include "Offset.h"
#include "Module.h"

namespace MemoryTools {
	inline bool g_bResolveRelative = true;
	void* FindPattern(const char*, const char*, const char* szName = nullptr) noexcept;
	DWORD CalculateOffset(const char*, const char*, DWORD dwOffset = 0, const char* szName = nullptr) noexcept;

	__forceinline void PlaceJMP(BYTE* bt_DetourAddress, DWORD dw_FunctionAddress, DWORD dw_Size)
	{
		DWORD dw_OldProtection, dw_Distance;
		VirtualProtect(bt_DetourAddress, dw_Size, PAGE_EXECUTE_READWRITE, &dw_OldProtection);
		dw_Distance = (DWORD)(dw_FunctionAddress - (DWORD)bt_DetourAddress) - 5;
		*bt_DetourAddress = 0xE9;
		*(DWORD*)(bt_DetourAddress + 0x1) = dw_Distance;
		for (int i = 0x5; i < dw_Size; i++) *(bt_DetourAddress + i) = 0x90;
		VirtualProtect(bt_DetourAddress, dw_Size, dw_OldProtection, NULL);
		return;
	}
	__forceinline void PlaceCALL(BYTE* bt_DetourAddress, DWORD dw_FunctionAddress, DWORD dw_Size)
	{
		DWORD dw_OldProtection, dw_Distance;
		VirtualProtect(bt_DetourAddress, dw_Size, PAGE_EXECUTE_READWRITE, &dw_OldProtection);
		dw_Distance = (DWORD)(dw_FunctionAddress - (DWORD)bt_DetourAddress) - 5;
		*bt_DetourAddress = 0xE8;
		*(DWORD*)(bt_DetourAddress + 0x1) = dw_Distance;
		for (int i = 0x5; i < dw_Size; i++) *(bt_DetourAddress + i) = 0x90;
		VirtualProtect(bt_DetourAddress, dw_Size, dw_OldProtection, NULL);
		return;
	}
	
	void* FindPattern(Modules, const char*, DWORD dwOffset = NULL, const char* szName = nullptr) noexcept;
	DWORD CalculateOffset(Modules, const char*, DWORD dwOffset = 0, const char* szName = nullptr) noexcept;


	template <class T>
	__forceinline T* FindPattern(Modules nModule, const char* szPattern, DWORD dwOffset = NULL, const char* szName = nullptr) noexcept
	{
		return reinterpret_cast<T*>(FindPattern(nModule, szPattern, dwOffset, szName));
	}


	template <class T>
	__forceinline T FindPattern(const char* szModule, const char* szPattern,  const char* szName = nullptr) noexcept
	{
		return reinterpret_cast<T>(FindPattern(szModule, szPattern, szName));
	}
	template <typename T>
	static constexpr auto _relativeToAbsolute(uintptr_t address) noexcept
	{
		return (T)(address + 4 + *reinterpret_cast<std::int32_t*>(address));
	}


	void HexPrint(const char* desc,	const void* addr,const int len,int perLine);

};


class Memory {
public:
	Memory();

//#ifdef _DEBUG
#if 1
	void AddOffset(Offset k_Offset);
	void WriteOffsets();
#endif
	DWORD ReadOffset(Offset nOffset);
	__declspec(noinline) bool ReadOffsets(std::string Data);
	Offset GetTrueOffset(Offset nOffset);
	DWORD GetOffset(Offset nOffset) { return m_arrOffset[GetTrueOffset(nOffset)]->GetOffset(); }
	char* GetAddress(Offset nOffset) { return m_arrOffset[GetTrueOffset(nOffset)]->GetAddress(); }
	StaticOffset* GetStaticOffset(Offset nOffset) { return m_arrOffset[GetTrueOffset(nOffset)]; }
private:
	StaticOffset* m_arrOffset[OFFSETMAX];

	std::fstream g_OffsetFile;
};

inline Memory* g_pMemory;
#include "Offset.h"
#include "Memory.h"

//#ifdef _DEBUG
#if 1
StaticOffset::StaticOffset(Modules nModule, const char* szPattern, DWORD dwAddressOffset, const char* szName) : m_szPattern(szPattern), m_szName(szName), m_nModule(nModule)
{
	m_dwOffset = MemoryTools::CalculateOffset(nModule, szPattern, dwAddressOffset, szName);
	m_pAddress = m_dwOffset + g_pModules->GetModule(nModule)->GetModuleBase();
}

StaticOffset::StaticOffset(Modules nModule, DWORD dwOffset, const char* szName) : m_dwOffset(dwOffset), m_szName(szName), m_nModule(nModule)
{
	m_pAddress = m_dwOffset + g_pModules->GetModule(nModule)->GetModuleBase();
}

StaticOffset::StaticOffset(Modules nModule, char* pAddress, const char* szName) : m_pAddress(pAddress), m_szName(szName), m_nModule(nModule)
{
	 m_dwOffset = (DWORD)(m_pAddress - (char*)(g_pModules->GetModule(nModule)->GetModuleBase()));
}


#else 
StaticOffset::StaticOffset(Modules nModule, const char* szPattern, const char* szName, DWORD dwAddressOffset)
{
	m_dwOffset = MemoryTools::CalculateOffset(nModule, szPattern, dwAddressOffset, szName);
	m_pAddress = g_pModules->GetModuleBase(nModule);
}

StaticOffset::StaticOffset(Modules nModule, DWORD dwOffset /* = 0*/) : m_nModule(nModule)
{
	m_pAddress = (char*)g_pModules->GetModuleBase(nModule) + m_dwOffset;
}

#endif



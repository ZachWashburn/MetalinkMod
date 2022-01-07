//====== Copyright c 1996-2007, Valve Corporation, All rights reserved. =======//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//

#include "pch_tier0.h"

#ifdef PLATFORM_WINDOWS_PC
#include "tier0/valve_off.h"
#include <windows.h>
#include <tchar.h>
#include "tier0/valve_on.h"
#endif

#include "tier0/platform.h"
#include "tier0/systeminformation.h"

// NOTE: This has to be the last file included!
#include "tier0/memdbgon.h"


#ifdef PLATFORM_WINDOWS_PC 

#ifdef __cplusplus

#endif



//
//	Cached information about a dll proc
//
class CSysCallCacheEntry
{
public:
	CSysCallCacheEntry();
	~CSysCallCacheEntry();

public:
	bool IsInitialized() const;
	SYSTEM_CALL_RESULT_t CallResult() const;

	SYSTEM_CALL_RESULT_t InitializeLoadModule( _TCHAR *pszModule, char *pszFunction );
	SYSTEM_CALL_RESULT_t InitializeFindModule( _TCHAR *pszModule, char *pszFunction );
	SYSTEM_CALL_RESULT_t InitializeFindProc( HMODULE hModule, char *pszFunction );

	void SetFailed( SYSTEM_CALL_RESULT_t eResult );
	void Reset();

	//template < typename FN >
	//FN GetFunction() const;

protected:
	SYSTEM_CALL_RESULT_t m_eResult;
	FARPROC m_pfnSysCall;
	HMODULE m_hModule;
	bool m_bInitialized;
	bool m_bFreeModule;
};

struct CSysCallCacheEntry_LoadModule : public CSysCallCacheEntry
{
	CSysCallCacheEntry_LoadModule( _TCHAR *pszModule, char *pszFunction ) { InitializeLoadModule( pszModule, pszFunction ); }
};
struct CSysCallCacheEntry_FindModule : public CSysCallCacheEntry
{
	CSysCallCacheEntry_FindModule( _TCHAR *pszModule, char *pszFunction ) { InitializeFindModule( pszModule, pszFunction ); }
};
struct CSysCallCacheEntry_FindProc : public CSysCallCacheEntry
{
	CSysCallCacheEntry_FindProc( HMODULE hModule, char *pszFunction ) { InitializeFindProc( hModule, pszFunction ); }
};



CSysCallCacheEntry::CSysCallCacheEntry() :
	m_eResult( SYSCALL_SUCCESS ),
	m_pfnSysCall( NULL ),
	m_hModule( NULL ),
	m_bInitialized( false ),
	m_bFreeModule( false )
{
	NULL;
}

CSysCallCacheEntry::~CSysCallCacheEntry()
{
	Reset();
}

bool CSysCallCacheEntry::IsInitialized() const
{
	return m_bInitialized;
}

SYSTEM_CALL_RESULT_t CSysCallCacheEntry::CallResult() const
{
	return m_eResult;
}

SYSTEM_CALL_RESULT_t CSysCallCacheEntry::InitializeLoadModule( _TCHAR *pszModule, char *pszFunction )
{
	m_bInitialized = true;

	m_hModule = ::LoadLibrary( pszModule );
	m_bFreeModule = true;
	if ( !m_hModule )
		return m_eResult = SYSCALL_NODLL;

	return InitializeFindProc( m_hModule, pszFunction );
}

SYSTEM_CALL_RESULT_t CSysCallCacheEntry::InitializeFindModule( _TCHAR *pszModule, char *pszFunction )
{
	m_bInitialized = true;

	m_hModule = ::GetModuleHandle( pszModule );
	m_bFreeModule = false;
	if ( !m_hModule )
		return m_eResult = SYSCALL_NODLL;

	return InitializeFindProc( m_hModule, pszFunction );
}

SYSTEM_CALL_RESULT_t CSysCallCacheEntry::InitializeFindProc( HMODULE hModule, char *pszFunction )
{
	m_bInitialized = true;

	m_pfnSysCall = GetProcAddress( hModule, pszFunction );
	if ( !m_pfnSysCall )
		return m_eResult = SYSCALL_NOPROC;

	return m_eResult = SYSCALL_SUCCESS;
}

void CSysCallCacheEntry::Reset()
{
	if ( m_bInitialized )
	{
		if ( m_bFreeModule && m_hModule )
			::FreeLibrary( m_hModule );
		m_eResult = SYSCALL_SUCCESS;
		m_hModule = NULL;
		m_pfnSysCall = NULL;
		m_bFreeModule = false;
		m_bInitialized = false;
	}
}

void CSysCallCacheEntry::SetFailed( SYSTEM_CALL_RESULT_t eResult )
{
	m_eResult = eResult;
}

//template < typename FN >
//FN CSysCallCacheEntry::GetFunction() const
//{
//	return reinterpret_cast< FN >( m_pfnSysCall );
//}



//
//	Plat_GetMemPageSize
//		Returns the size of a memory page in bytes.
//


//
//	Plat_GetPagedPoolInfo
//		Fills in the paged pool info structure if successful.
//
#if 0
SYSTEM_CALL_RESULT_t Plat_GetPagedPoolInfo( PAGED_POOL_INFO_t *pPPI )
{
	memset( pPPI, 0, sizeof( *pPPI ) );

	static CSysCallCacheEntry_FindModule qsi( (char*)_T( "ntdll.dll" ), "NtQuerySystemInformation" );
	
	if ( qsi.CallResult() != SYSCALL_SUCCESS )
		return qsi.CallResult();

	static bool s_bOsVersionValid = false;
	if ( !s_bOsVersionValid )
	{
		s_bOsVersionValid = true;
		OSVERSIONINFO osver;
		memset( &osver, 0, sizeof( osver ) );
		osver.dwOSVersionInfoSize = sizeof( osver );
		GetVersionEx( &osver );

		// We should run it only on Windows XP or Windows 2003
#define MAKEVER( high, low ) DWORD( MAKELONG( low, high ) )
		DWORD dwOsVer = MAKEVER( osver.dwMajorVersion, osver.dwMinorVersion );
		if ( dwOsVer < MAKEVER( 5, 1 ) ||	// Earlier than WinXP
			 dwOsVer > MAKEVER( 5, 2 ) )	// Later than Win2003 (or 64-bit)
		{
			qsi.SetFailed( SYSCALL_UNSUPPORTED );
		}

		// Don't care for 64-bit Windows
		CSysCallCacheEntry_FindModule wow64( _T( "kernel32.dll" ), "IsWow64Process" );
		if ( wow64.CallResult() == SYSCALL_SUCCESS )
		{
			typedef BOOL ( WINAPI * PFNWOW64 )( HANDLE, PBOOL );
			BOOL b64 = FALSE;
			if ( ( wow64.GetFunction< PFNWOW64 >() )( GetCurrentProcess(), &b64 ) &&
				 b64 )
			{
				qsi.SetFailed( SYSCALL_UNSUPPORTED );
			}
		}
		
		if ( qsi.CallResult() != SYSCALL_SUCCESS )
			return qsi.CallResult();
	}

	// Invoke proc
	PrivateType( SYSTEM_PERFORMANCE_INFORMATION ) spi;
	ULONG ulLength = sizeof( spi );
	PrivateType( NTSTATUS ) lResult =
		( qsi.GetFunction< PrivateType( NtQuerySystemInformation ) >() )
		( SystemPerformanceInformation, &spi, ulLength, &ulLength );
	if ( lResult )
		return SYSCALL_FAILED;

	// Return the result
	pPPI->numPagesUsed = spi.PagedPoolPages;
	pPPI->numPagesFree = spi.FreePagedPoolPages;
	return SYSCALL_SUCCESS;
}


#else


//
//	Plat_GetMemPageSize
//		Returns the size of a memory page in bytes.
//

//
//	Plat_GetPagedPoolInfo
//		Fills in the paged pool info structure if successful.
//
SYSTEM_CALL_RESULT_t Plat_GetPagedPoolInfo( PAGED_POOL_INFO_t *pPPI )
{
	memset( pPPI, 0, sizeof( *pPPI ) );
	return SYSCALL_UNSUPPORTED;
}
#endif

#endif


#include "../BaseIncludes.h"
#include "tier0/memalloc.h"

#if _MSC_VER >= 1400

#if _MSC_VER >= 1900
#define _CRTNOALIAS
#endif

#define ALLOC_CALL _CRTNOALIAS _CRTRESTRICT 
#define FREE_CALL _CRTNOALIAS 
#else
#define ALLOC_CALL
#define FREE_CALL
#endif

void InitializeAllocator() {
#ifdef OVERRIDEALLOCATOR
	g_pMemAlloc = *(IMemAlloc**)GetProcAddress(GetModuleHandleA(XorStr("tier0")), XorStr("g_pMemAlloc"));
#endif
}


#ifdef OVERRIDEALLOCATOR

#if defined(USE_MEM_DEBUG)
#pragma optimize( "", off )
#define inline
#endif


inline void* AllocUnattributed(size_t nSize)
{
#if !defined(USE_LIGHT_MEM_DEBUG) && !defined(USE_MEM_DEBUG)
	return g_pMemAlloc->Alloc(nSize);
#else
	return MemAlloc_Alloc(nSize, ::g_pszModule, 0);
#endif
}

inline void* ReallocUnattributed(void* pMem, size_t nSize)
{
	if (!pMem)
		return AllocUnattributed(nSize);
	return g_pMemAlloc->Realloc(pMem, nSize);
}

#undef inline



extern "C"
{

	ALLOC_CALL void* malloc(size_t nSize)
	{
		return AllocUnattributed(nSize);
	}

	FREE_CALL void free(void* pMem)
	{
#if !defined(USE_LIGHT_MEM_DEBUG) && !defined(USE_MEM_DEBUG)
		g_pMemAlloc->Free(pMem);
#else
		g_pMemAlloc->Free(pMem, ::g_pszModule, 0);
#endif
	}

	ALLOC_CALL void* realloc(void* pMem, size_t nSize)
	{
		return ReallocUnattributed(pMem, nSize);
	}

	ALLOC_CALL void* calloc(size_t nCount, size_t nElementSize)
	{
		void* pMem = AllocUnattributed(nElementSize * nCount);
		memset(pMem, 0, nElementSize * nCount);
		return pMem;
	}

} // end extern "C"
#if 0
void* malloc(size_t nSize)
{
	return g_pMemAlloc->Alloc(nSize);
}

void* realloc(void* ptr, size_t size)
{
	return g_pMemAlloc->Realloc(ptr, size);
}

void free(void* ptr)
{
	return g_pMemAlloc->Free(ptr);
}
#endif
extern "C"
{

	// 64-bit
#ifdef _WIN64
	void* __cdecl _malloc_base(size_t nSize)
	{
		return AllocUnattributed(nSize);
	}
#else
	void* _malloc_base(size_t nSize)
	{
		return AllocUnattributed(nSize);
	}
#endif

#if ( defined ( _MSC_VER ) && _MSC_VER >= 1900 )
	_CRTRESTRICT void* _calloc_base(size_t nCount, size_t nSize)
	{
		void* pMem = AllocUnattributed(nCount * nSize);
		memset(pMem, 0, nCount * nSize);
		return pMem;
	}
#else
	void* _calloc_base(size_t nSize)
	{
		void* pMem = AllocUnattributed(nSize);
		memset(pMem, 0, nSize);
		return pMem;
	}
#endif

	void* _realloc_base(void* pMem, size_t nSize)
	{
		return ReallocUnattributed(pMem, nSize);

	}

#if 1
	_CRTRESTRICT void* _recalloc_base(void* pMem, size_t nCount, size_t nSize)
	{
		void* pMemOut = ReallocUnattributed(pMem, nSize);
		if (!pMem)
		{
			memset(pMemOut, 0, nSize);
		}
		return pMemOut;
	}
#endif

	void _free_base(void* pMem)
	{
#if !defined(USE_LIGHT_MEM_DEBUG) && !defined(USE_MEM_DEBUG)
		g_pMemAlloc->Free(pMem);
#else
		g_pMemAlloc->Free(pMem, ::g_pszModule, 0);
#endif
	}

	void* __cdecl _expand_base(void* pMem, size_t nNewSize, int nBlockUse)
	{
		Assert(0);
		return NULL;
	}

	// crt
	void* __cdecl _malloc_crt(size_t size)
	{
		return AllocUnattributed(size);
	}

	void* __cdecl _calloc_crt(size_t count, size_t size)
	{
#if (defined( _MSC_VER ) && _MSC_VER >= 1900)
		return _calloc_base(count, size);
#else
		return _calloc_base(count * size);
#endif
	}

	void* __cdecl _realloc_crt(void* ptr, size_t size)
	{
		return _realloc_base(ptr, size);
	}

	void* __cdecl _recalloc_crt(void* ptr, size_t count, size_t size)
	{
		return _recalloc_base(ptr, count, size);
	}

	ALLOC_CALL void* __cdecl _recalloc(void* memblock, size_t count, size_t size)
	{
		void* pMem = ReallocUnattributed(memblock, size * count);
		if (!memblock)
		{
			memset(pMem, 0, size * count);
		}
		return pMem;
	}

	size_t _msize_base(void* pMem)
	{
		return g_pMemAlloc->GetSize(pMem);
	}

	size_t _msize(void* pMem)
	{
		return _msize_base(pMem);
	}

	size_t msize(void* pMem)
	{
		return g_pMemAlloc->GetSize(pMem);
	}

	void* __cdecl _heap_alloc(size_t nSize)
	{
		return AllocUnattributed(nSize);
	}

	void* __cdecl _nh_malloc(size_t nSize, int)
	{
		return AllocUnattributed(nSize);
	}

	void* __cdecl _expand(void* pMem, size_t nSize)
	{
		Assert(0);
		return NULL;
	}

	unsigned int _amblksiz = 16; //BYTES_PER_PARA;

#if _MSC_VER >= 1400
	HANDLE _crtheap = (HANDLE)1;	// PatM Can't be 0 or CRT pukes
	int __active_heap = 1;
#endif //  _MSC_VER >= 1400

	size_t __cdecl _get_sbh_threshold(void)
	{
		return 0;
	}

	int __cdecl _set_sbh_threshold(size_t)
	{
		return 0;
	}

	int _heapchk()
	{
		return g_pMemAlloc->heapchk();
	}

	int _heapmin()
	{
		return 1;
	}

	int __cdecl _heapadd(void*, size_t)
	{
		return 0;
	}

	int __cdecl _heapset(unsigned int)
	{
		return 0;
	}

	size_t __cdecl _heapused(size_t*, size_t*)
	{
		return 0;
	}

#ifdef _WIN32
#include <malloc.h>
	int __cdecl _heapwalk(_HEAPINFO*)
	{
		return 0;
	}
#endif

} // end extern "C"
#ifdef _WIN32

extern "C"
{
	size_t __crtDebugFillThreshold = 0;

#ifdef _DEBUG
	void __cdecl _free_dbg(
		_Pre_maybenull_ _Post_invalid_ void* _Block,
		_In_                           int   _BlockUse
	)
	{
		g_pMemAlloc->Free(_Block);
	}
#endif

	extern "C" void* __cdecl _heap_alloc_base(size_t size)
	{
		Assert(0);
		return NULL;
	}


	void* __cdecl _heap_alloc_dbg(size_t nSize, int nBlockUse, const char* szFileName, int nLine)
	{
		return _heap_alloc(nSize);
	}

	// 64-bit
#ifdef _WIN64
	static void* __cdecl realloc_help(void* pUserData, size_t* pnNewSize, int nBlockUse, const char* szFileName,
		int nLine, int fRealloc)
	{
		Assert(0); // Shouldn't be needed
		return NULL;
	}
#else
	static void* __cdecl realloc_help(void* pUserData, size_t nNewSize, int nBlockUse, const char* szFileName,
		int nLine, int fRealloc)
	{
		Assert(0); // Shouldn't be needed
		return NULL;
	}
#endif

	void __cdecl _free_nolock(void* pUserData)
	{
		// I don't think the second param is used in memoverride
		_free_dbg(pUserData, 0);
	}

	void __cdecl _free_dbg_nolock(void* pUserData, int nBlockUse)
	{
		_free_dbg(pUserData, 0);
	}
#ifdef _DEBUG
	_CRT_ALLOC_HOOK __cdecl _CrtGetAllocHook(void)
	{
		Assert(0);
		return NULL;
	}
#endif

	static int __cdecl CheckBytes(unsigned char* pb, unsigned char bCheck, size_t nSize)
	{
		int bOkay = TRUE;
		return bOkay;
	}

#ifdef _DEBUG
	_CRT_DUMP_CLIENT __cdecl _CrtGetDumpClient(void)
	{
		Assert(0);
		return NULL;
	}
#endif
#if _MSC_VER >= 1400
	static void __cdecl _printMemBlockData(_locale_t plocinfo, _CrtMemBlockHeader* pHead)
	{
	}

	static void __cdecl _CrtMemDumpAllObjectsSince_stat(const _CrtMemState* state, _locale_t plocinfo)
	{
	}
#endif
	void* __cdecl _aligned_malloc_dbg(size_t size, size_t align, const char* f_name, int line_n)
	{
		return _aligned_malloc(size, align);
	}

	void* __cdecl _aligned_realloc_dbg(void* memblock, size_t size, size_t align,
		const char* f_name, int line_n)
	{
		return _aligned_realloc(memblock, size, align);
	}

	void* __cdecl _aligned_offset_malloc_dbg(size_t size, size_t align, size_t offset,
		const char* f_name, int line_n)
	{
		return _aligned_offset_malloc(size, align, offset);
	}

	void* __cdecl _aligned_offset_realloc_dbg(void* memblock, size_t size, size_t align,
		size_t offset, const char* f_name, int line_n)
	{
		return _aligned_offset_realloc(memblock, size, align, offset);
	}

	void __cdecl _aligned_free_dbg(void* memblock)
	{
		_aligned_free(memblock);
	}

#if _MSC_VER < 1900
	size_t __cdecl _CrtSetDebugFillThreshold(size_t _NewDebugFillThreshold)
	{
		Assert(0);
		return 0;
	}

#endif
}
#endif
#endif
#pragma once
#include "../PREPROCESSOR.h"

class IMemAlloc;
inline IMemAlloc* g_pMemAlloc;

#ifdef OVERRIDEALLOCATOR // It does work, but enabling it requires you to go through and fix a couple crashes everytime you add something. oh well

void InitializeAllocator();

class InitAllocator {
public:
	InitAllocator() { InitializeAllocator(); }
};
inline InitAllocator g_AllocatorInitializer;

#if 0
void* malloc(size_t size);
void* realloc(void* ptr, size_t size);
void free(void* ptr);
#endif
extern "C"
{
	 void* malloc(size_t nSize);
	 void free(void* pMem);
	 void* realloc(void* pMem, size_t nSize);
	 void* calloc(size_t nCount, size_t nElementSize);
} // end extern "C"
#endif
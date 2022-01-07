#pragma once



#pragma once



#ifndef COMPILER_DEFINITIONS_H
#define COMPILER_DEFINITIONS_H

#define CSGO
//#define MEM_OVERRIDE_ON
//#define OVERRIDEALLOCATOR
// /FORCE:MULTIPLE 
#define MEMORY_DEBUGGING
#define OVERRIDE_WINDOWS_API
//#define D3D_DEBUG_INFO

#define _SILENCE_ALL_CXX20_DEPRECATION_WARNINGS
#define _SILENCE_CXX20_IS_POD_DEPRECATION_WARNING
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS



#define SECURE
#ifdef SECURE
#define XOR_STRINGS
//#define METAMORPHIC
#else
#endif

//#undef XOR_STRINGS

//#pragma error( disable : 2397 ) /* FIX!!!!! */
//#define IMGUI_MENU
#ifdef XOR_STRINGS
#include "ThirdParty/XOR_STR.h"
#else
#define XorStr(VAR) VAR
#define XorStrW(VAR) VAR
#endif

#ifdef METAMORPHIC
#include "MetaMorphic.h"
#else
#define JUNKCODE /**/
#endif

#define exceptionOnError
#ifdef exceptionOnError
#define FATAL_ERROR { DEBUG_OUT_STR_VAR("Fatal Error At %d\n",__LINE__); throw std::exception("FATAL ERROR OCCURED\n");}
#else
#define FATAL_ERROR /**/
#endif

//#define _LIVE
#ifdef _DEBUG
#define DEBUGNOINLINE __declspec(noinline)
#define _ASSERT(COND) {if(!(COND)){DEBUG_OUT_STR_VAR("Fatal Error At %d in %s, !(%s)\n",__LINE__, __FILE__, #COND); __debugbreak(); LOGCRITICAL(#COND); exit(0); } }

#define DEBUGSTR(STR) XorStr(STR)
#define DEBUG_OUT_STR(VAR) OutputDebugStringA(VAR); LOG(VAR);
#define X_DEBUG_OUT_STR(VAR) OutputDebugStringA(XorStr(VAR)); LOG(XorStr(VAR));

#define DEBUG_OUT_STDSTR(VAR) OutputDebugStringA(VAR.c_str());
#define DEBUG_OUT_STR_VAR(VAR, ...) Debug::Varidic_Debug_Out(XorStr(VAR), __VA_ARGS__);
#define DEBUG_OUT_EXIT_ERROR(VAR) {DEBUG_OUT_STR(VAR); return false;}
#define FUNCTION_DEBUG(FUNCTION_NAME, FORMATTER, ...) Debug::Function_Out(FUNCTION_NAME, FORMATTER, __VA_ARGS__);  
#else

#ifdef _LIVE
#define _ASSRT(COND) ;
#else 
#define _ASSERT(COND) if(!(COND)){QuickCrash;}
#endif
#define DEBUGNOINLINE /**/
#define DEBUGSTR(STR) ""
#define DEBUG_OUT_STR(VAR) ;
#define X_DEBUG_OUT_STR(VAR) ;

#define DEBUG_OUT_STDSTR(VAR) ;
#define FUNCTION_DEBUG(FUNCTION_NAME, FORMATTER, ...) ;
#define DEBUG_OUT_STR_VAR(VAR, ...) ;
#define DEBUG_OUT_EXIT_ERROR(VAR) return false;
#define FUNCTION_DEBUG(FUNCTION_NAME, FORMATTER, ...) ;  

#endif

#define DEBUG_ONLY_START #ifdef _DEBUG
#define DEBUG_ONLY_END #endif

#ifdef CSGO
/* CSGO SPECIFIC */

#define Error(ERR) X_DEBUG_OUT_STR(ERR); 
#define Assert(L) _ASSERT(L);
#define AssertMsg(L) _ASSERT(L); 
#define WARNING(L) X_DEBUG_OUT_STR(L); 
#define Warning(L) X_DEBUG_OUT_STR(L); 
#define Msg(L) X_DEBUG_OUT_STR(L); 
#define AssertMsg1(L) _ASSERT(L); 
#define AssertMsg2(L) _ASSERT(L); 
#define AssertMsg3(L) _ASSERT(L); 

#define COMPILER_MSVC
#define COMPILER_MSVC32



//


#define CONSOLE_OUT_COLOR(OUT_MSG, R,G,B,A) Debug::RAW_ConsolePrint(OUT_MSG, R, G, B, A);
#define CON_OUT(OUT_MSG) CONSOLE_OUT_COLOR(OUT_MSG,176,117,255,255);
#define CON(OUT_MSG) Debug::ConsolePrint(OUT_MSG);





#define VCON(OUT_MSG, ...) Debug::ConsolePrintV(OUT_MSG, __VA_ARGS__);
#define VCON_RAW(OUT_MSG, ...) Debug::ConsolePrintV_Raw(OUT_MSG, __VA_ARGS__);
#define COLORCON(OUT_MSG,R,G,B,A) Debug::ConsolePrint(OUT_MSG, R,G,B,A);


//#define EXPLOITS
#define INCLUDE_PUBLIC_CRASHER
#define DEV_SECRET_BUILD
#define GAME_BREAK_EXPLOITS



#define COMBINE1(X,Y) X##Y  // helper macro
#define COMBINE(X,Y) COMBINE1(X,Y)

#define SUBFUNC() virtual void COMBINE(sub_,__LINE__ )() = 0;
#define FUNC_PLACEHOLDER(VAR) void COMBINE(Func, __LINE__)();
#define ClassFunction __forceinline


#define PAD(AMMOUNT) char COMBINE(pad_, __COUNTER__)[AMMOUNT];


// Other
#ifdef _DEBUG
#define QuickCrash { __debugbreak(); int nZero = 0; nZero = nZero / nZero; };
#else
#ifndef LIVE
#define QuickCrash { if(IsDebuggerPresent()){__debugbreak();} int nZero = 0; nZero = nZero / nZero; };
#else
#define QuickCrash { int nZero = 0; nZero = nZero / nZero; };
#endif

#endif



#endif
#endif




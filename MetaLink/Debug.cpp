#pragma once

#include "BaseIncludes.h"

#include "Memory.h"
#include "FunctionTypeDeclares.h"
namespace Debug {
#ifdef _DEBUG
	bool __stdcall Function_Out(const char* szFunctionName, const char* szFormat, ...) {
		va_list args;
		va_start(args, szFormat);
		char buffer1[4096 + 1024];
		char buffer[4096];
		strcpy_s(buffer1, "Function Call To : ");
		strcat_s(buffer1, szFunctionName);
		strcat_s(buffer1, " : (");
		vsprintf(buffer, szFormat, args);
		strcat_s(buffer1, buffer);
		strcat_s(buffer1, ")\n");
		DEBUG_OUT_STR(buffer1);
		va_end(args);
		return true;
	}


	void __stdcall Varidic_Debug_Out(const char* szFormatter, ...) {
		va_list args;
		va_start(args, szFormatter);
		char buffer[4096];
		vsprintf(buffer, szFormatter, args);
		DEBUG_OUT_STR(buffer);
		va_end(args);
	}


	void QuickPrint(const char* szIn) {
		DEBUG_OUT_STR(szIn)
	}

#else 
	inline bool Function_Out(const char*, const char*, ...) { return false; };
	inline void Varidic_Debug_Out(const char*, ...) {};
	inline void QuickPrint() { ; }

	void QuickPrint(const char* szIn) {
		DEBUG_OUT_STR(szIn)
	}
#endif
	void RAW_ConsolePrint(const char* szIn, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
	{
		static ColorConMsgFunc_t ColorConMsg{ reinterpret_cast<ColorConMsgFunc_t>(GetProcAddress(GetModuleHandle(XorStr("tier0")), XorStr("?ConColorMsg@@YAXABVColor@@PBDZZ"))) };
		ColorConMsg({ r,g,b,a }, szIn);
	}




	void ConsolePrint(const char* szIn, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		char buffer[4096];
		const char* prefix = "[GG] ";
		strcpy_s(buffer, prefix);
		strcat_s(buffer, szIn);
		CONSOLE_OUT_COLOR(buffer, r, g, b, a);
	}


	void ConsolePrintV(const char* szIn, ...) {
		char buffer2[4000];
		char buffer[4096];

		va_list args;
		va_start(args, szIn);
		vsprintf(buffer2, szIn, args);
		va_end(args);

		const char* prefix = "[A] ";
		strcpy_s(buffer, prefix);
		strcpy_s(buffer, buffer2);
		ConsolePrint(buffer, 176,  117,  255,  255);
	}

	void ConsolePrint_RAW(const char* szIn, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		CONSOLE_OUT_COLOR(szIn, r, g, b, a);
	}

	void ConsolePrintV_Raw(const char* szIn, ...) {
		char buffer2[4000];
		char buffer[4096];
		va_list args;
		va_start(args, szIn);
		vsprintf(buffer2, szIn, args);
		va_end(args);
		strcpy_s(buffer, buffer2);
		ConsolePrint_RAW(buffer, 176, 117, 255, 255);
	}


}
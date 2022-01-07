#pragma once


#include "PREPROCESSOR.h"


namespace Debug {
	bool Function_Out(const char* szFunctionName, const char* szFormat, ...);

	void Varidic_Debug_Out(const char* szFormatter, ...);
	void QuickPrint(const char* szIn);


	void RAW_ConsolePrint(const char* szIn, uint8_t r = 176, uint8_t g = 117, uint8_t b = 255, uint8_t a = 255);

	void ConsolePrint(const char* szIn, uint8_t r = 176, uint8_t g = 117, uint8_t b = 255, uint8_t a = 255);
	//void ConsolePrint(char* szIn, uint8_t r = 176, uint8_t g = 117, uint8_t b = 255, uint8_t a = 255);
	void ConsolePrintV(const char* szIn, ...);
	void ConsolePrintV_Raw(const char* szIn, ...);
}

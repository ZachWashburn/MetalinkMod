#pragma once
#include "../BaseIncludes.h"





using KeyValuesSystemFn = void* (__cdecl*)();
class KeyValues {
public:
	static KeyValues* FromString(const char* name, const char* value) noexcept;
	KeyValues* FindKey(const char* keyName, bool create = false) noexcept;
	void SetString(const char* keyName, const char* value) noexcept;
	const char* GetName() noexcept;

	int   GetInt(const char* keyName = NULL, int defaultValue = 0);
	//uint64_t GetUint64(const char* keyName = NULL, uint64_t defaultValue = 0);
	float GetFloat(const char* keyName = NULL, float defaultValue = 0.0f);
	const char* GetString(const char* keyName = NULL, const char* defaultValue = "");
};

void SetupKeyValues();


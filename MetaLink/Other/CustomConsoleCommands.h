#include "../BaseIncludes.h"
#include "SDK.h"
#include "tier1/convar.h"

class CustomConvar 
{
public:
	CustomConvar(){
		
	}
private:
	std::byte pad[24];
	std::add_pointer_t<void __cdecl()> changeCallback;
	ConVar* m_pParent;
	const char* m_pszDefaultValue;
	char* m_szStringValue;
	int							m_StringLength;
	// Values
	float						m_fValue;
	int							m_nValue;
	bool						m_bHasMin;
	float						m_fMinVal;
	bool						m_bHasMax;
	float						m_fMaxVal;
	// Call this function when ConVar changes
	CUtlVector< FnChangeCallback_t > m_fnChangeCallbacks;
};


class CustomConCommand
{
public:
	PAD(0x20); 
	char m_bCallbackType;
};



typedef void(*FnCommandCallbackVoid_t)(void);
typedef void(*FnCommandCallback_t)(const CCommand& command);
#define COMMAND_COMPLETION_MAXITEMS		64
#define COMMAND_COMPLETION_ITEM_LENGTH	64
typedef int(*FnCommandCompletionCallback)(const char* partial, char commands[COMMAND_COMPLETION_MAXITEMS][COMMAND_COMPLETION_ITEM_LENGTH]);

enum CommandCallbackTypes
{
	TYPE_VERSION1 = 1, //legacy
	TYPE_VERSION1_ARG = 2,
	TYPE_VERSION2 = 4
};

void InitalizeOurConvars();
void InitalizeOurConCommands();

inline float g_dbDesiredFrameRate = 0.f; // Find A better place for this

//typedef void(*FnChangeCallback_t)(const CCommand& command);
//std::uintptr_t CreateCustomConvar
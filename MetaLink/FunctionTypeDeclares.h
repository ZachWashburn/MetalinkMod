#pragma once
//#include "BaseIncludes.h"
/* For Function Types */
#include "WindowsInclude.h"
#include <array>
class studiohdr_t;
class Entity;
class INetChannel;
class CIKContext;
struct netpacket_s;
enum DataUpdateType_t;
class CBoneBitList;
class CStudioHdr;
class Vector;
class QAngle;
class Quaternion;
class QuaternionAligned;
class matrix3x4_t;
class C_BaseAnimating;
typedef void(__cdecl* HostRunFrameInputFunc_t)(float, bool);
typedef void(__cdecl* CL_MoveFunc_t)(float, bool);
typedef __int32(__cdecl* CBufExecuteFunc_t)(void);
typedef double(__cdecl* Plat_FloatTimeFunc_t)();
typedef void(__stdcall* ProcessInputFunc_t)(bool);
typedef void(__thiscall* SaveDataFunc_t)(void*, const char*, int, int);
typedef int(__fastcall* GetSequenceActivityFunc_t)(void*, studiohdr_t*, int);
typedef int(__thiscall* CBaseEntity_PostNetworkDataReceivedFunc_t)(Entity*, DataUpdateType_t);
typedef void(__cdecl* ColorConMsgFunc_t)(const std::array<std::uint8_t, 4>& color, const char* msg, ...);
typedef void(__thiscall* LockStudioHdrFunc_t)(Entity*);
typedef CIKContext*(__thiscall* CIKContextConstructorFunc_t)(void*);
typedef bool(__thiscall* TeleportedFunc_t)(Entity*);
typedef void(__thiscall* CIKConextInitFunc_t)(CIKContext*, CStudioHdr*, QAngle&, Vector&, float, int, int);
typedef void(__thiscall* StandardBlendingRulesFunc_t)(Entity*, CStudioHdr*, Vector[], QuaternionAligned[], float, int);
typedef void(__thiscall* CIKContextUpdateTargetsFunc_t)(CIKContext*, Vector*, Quaternion*, matrix3x4_t*, CBoneBitList&);
typedef void(__thiscall* CalculateIKLocksFunc_t)(Entity*, float);
typedef void(__thiscall* CIKContextSolveDependenciesFunc_t)(CIKContext*, Vector*, Quaternion*, matrix3x4_t*, CBoneBitList&);
typedef void(__thiscall* SetupBones_AttachmentHelperFunc_t)(C_BaseAnimating*, CStudioHdr*);
typedef void(__thiscall* SetReservationCookieFunc_t)(void*, unsigned long long);




// Original Defines 

inline SetReservationCookieFunc_t oSetReservationCookie;
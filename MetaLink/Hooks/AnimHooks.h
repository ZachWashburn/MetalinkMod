#pragma once
#include "../BaseIncludes.h"
#include "mathlib/mathlib.h"
class CStudioHdr;
class CIKContext;
class CBoneBitList;
class matrix3x4a_t;
class C_BaseAnimating;
class Entity;
typedef void(__thiscall* C_CSPlayerDoExtraBoneProcessingFunc_t)(Entity*, CStudioHdr*, BoneVector*, BoneQuaternion*, matrix3x4a_t*, CBoneBitList&, CIKContext*);
inline C_CSPlayerDoExtraBoneProcessingFunc_t oDoExtraBoneProcessing;
void __fastcall hk_C_CSPlayer_DoExtraBoneProcessing(Entity* pEnt, void* ecx, CStudioHdr* pStudioHdr, BoneVector pos[], BoneQuaternion q[], matrix3x4a_t boneToWorld[], CBoneBitList& boneComputed, CIKContext* pIKContext);


typedef bool(__thiscall* C_BaseAnimating_SetupBonesFunc_t)(C_BaseAnimating* pBaseAnimating, matrix3x4a_t* pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime);
inline C_BaseAnimating_SetupBonesFunc_t oSetupBones;
bool __fastcall hk_BaseAnimating_SetupBones(C_BaseAnimating* pBaseAnimating, void* ecx, matrix3x4a_t* pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime);


inline LPVOID oShouldSkipAnimationFrame = 0;
bool __fastcall hk_CBaseAnimating_ShouldSkipAnimationFrame(void* _this, void*);
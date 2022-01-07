#pragma once
#include "../BaseIncludes.h"
#include "mathlib/mathlib.h"
#include "VirtualFunction.h"
#include "Entity.h"
class CStudioHdr;
class CIKContext;
class Entity;
class CUtlVectorSimple;
class CThreadFastMutex;
#define ANIMATINGBASEENTITY (Entity*)((char*)this-(int)4)
class C_BaseAnimating
{
public:
	CStudioHdr* GetModelPtr();
	CIKContext* GetIKContext();
	CIKContext** GetIKContextPtr();
	CStudioHdr** GetModelPtrPtr();
	

	bool Teleported();

	int* GetEntClientFlagsPtr();

	Entity* GetBaseEntity() { return (Entity*)ANIMATINGBASEENTITY; }

	ClassFunction Vector& GetAbsOrigin() { return std::ref(CallVirtualFunction<Vector&>(ANIMATINGBASEENTITY, 10)); }
	ClassFunction QAngle& GetAbsAngle() { return std::ref(CallVirtualFunction<QAngle&>(ANIMATINGBASEENTITY, 11)); }
	ClassFunction int& m_fEffects() { return GetBaseEntity()->m_fEffects(); }

	ClassFunction bool IsLocalPlayer() { return GetBaseEntity()->IsLocalPlayer(); }
	ClassFunction bool& m_bClientSideRagdoll() { return std::ref(GetBaseEntity()->m_bClientSideRagdoll()); }
	ClassFunction void UpdateIKLocks(float currentTime) { return GetBaseEntity()->UpdateIKLocks(currentTime); }
	ClassFunction void CalculateIKLocks(float currentTime) { return GetBaseEntity()->CalculateIKLocks(currentTime); }
	ClassFunction void ControlMouth(CStudioHdr* hdr) { return GetBaseEntity()->ControlMouth(hdr); }
	ClassFunction bool DoExtraBoneProcessing(CStudioHdr* hdr, Vector* pos, Quaternion* q, matrix3x4_t* bonearray, CBoneBitList& computed, CIKContext* m_pIK) { return CallVirtualFunction<bool, CStudioHdr*, Vector*, Quaternion*, matrix3x4_t*, CBoneBitList&, CIKContext*>(ANIMATINGBASEENTITY, 197, hdr, pos, q, bonearray, std::ref(computed), m_pIK); }
	void FormatViewModelAttachment(int nAttachment, matrix3x4_t* attachmentToWorld);
	void BuildTransformations(CStudioHdr* hdr, BoneVector* pos, BoneQuaternion* q, const matrix3x4_t& cameraTransform, int boneMask, CBoneBitList* boneComputed);
	void SetupBones_AttachmentHelper(CStudioHdr*, matrix3x4a_t* customMatrix = nullptr);
	// Not Done Yet
	void BuildTransformationsRebuild(CStudioHdr* hdr, BoneVector* pos, BoneQuaternion* q, const matrix3x4_t& cameraTransform, int boneMask, CBoneBitList& boneComputed);
	bool PutAttachment(int number, const matrix3x4_t& attachmentToWorld);
	CUtlVectorSimple& GetAttachments();

	const matrix3x4a_t& GetBone(int iBone);
	CBoneAccessor* GetBoneAccessor();
	bool IsPlayer();
	void StandardBlendingRules(CStudioHdr* hdr, Vector* pPos, BoneQuaternionAligned* qa, float curTime, int bonemask);
	CThreadFastMutex* GetSetupBoneLock();
	matrix3x4a_t* GetCachedBoneBase();
	int GetCachedBoneCount();


	void AddEFlags(int nFlag);
	void RemoveEFlag(int nFlags);
private: // Don't use these, just here for debugging purposes. 
	PAD(10568);
	CStudioHdr* m_pHdr;
};
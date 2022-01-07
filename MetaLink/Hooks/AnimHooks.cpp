#include "AnimHooks.h"
#include "SDK.h"
#include "../Memory.h"
#include "../BaseIncludes.h"

void __fastcall hk_C_CSPlayer_DoExtraBoneProcessing(Entity* pEnt, void* ecx, CStudioHdr* pStudioHdr, BoneVector pos[], BoneQuaternion q[], matrix3x4a_t boneToWorld[], CBoneBitList& boneComputed, CIKContext* pIKContext) {
	// Call doesn't exist in server.dll setup bones


	return;
}




bool __fastcall hk_BaseAnimating_SetupBones(C_BaseAnimating* pBaseAnimating, void* ecx, matrix3x4a_t* pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime)
{
	if (!pBaseAnimating || !pBaseAnimating->GetBaseEntity()->IsPlayer()) // Sorry but we dont need to worry about chicken bones
		return oSetupBones(pBaseAnimating, pBoneToWorldOut, nMaxBones, boneMask, currentTime);
	else
	{

		CBoneAccessor* m_BoneAccessor = pBaseAnimating->GetBoneAccessor();
		PlayerRecord* pRecord = pBaseAnimating->GetBaseEntity()->GetPlayerRecord();
		if (pRecord)
		{
			if ((pRecord->LastSetupBones() >= gpGlobals->m_nTickcount) || (pRecord->LastUpdate() < gpGlobals->m_nTickcount))
			{
				if (pBoneToWorldOut)
				{
					Plat_FastMemcpy(pBoneToWorldOut, m_BoneAccessor->GetBoneArrayForWrite(), sizeof(matrix3x4a_t) * nMaxBones);
					return true;
				}
				return true;
			}
			else
			{
				if(pBoneToWorldOut)
					pRecord->BonesSetup(gpGlobals->m_nTickcount);
			}

			return oSetupBones(pBaseAnimating, pBoneToWorldOut, nMaxBones, pRecord->IsInThreadedBonesSetup() ? 0xFFFFFFFF : boneMask, currentTime);
		}	

		int nBoneMask = BONE_USED_BY_HITBOX;
		if (boneMask & BONE_USED_BY_VERTEX_MASK)
			nBoneMask |= BONE_USED_BY_VERTEX_MASK;
		if (boneMask & BONE_USED_BY_BONE_MERGE)
			nBoneMask |= BONE_USED_BY_BONE_MERGE;
		if (boneMask & BONE_USED_BY_ATTACHMENT)
			nBoneMask |= BONE_USED_BY_ATTACHMENT;
		if (boneMask & BONE_USED_BY_WEAPON)
			nBoneMask |= BONE_USED_BY_WEAPON;

		return oSetupBones(pBaseAnimating, pBoneToWorldOut, nMaxBones, nBoneMask, currentTime);
	}

	CThreadFastMutexLockInstance InstanceLock(pBaseAnimating->GetSetupBoneLock());

	static CIKContextConstructorFunc_t CreateIK{ reinterpret_cast<CIKContextConstructorFunc_t>(g_pMemory->GetAddress(__CIKCONTEXTCONSTRUCTOR)) };
	static CIKConextInitFunc_t InitIK(reinterpret_cast<CIKConextInitFunc_t>(g_pMemory->GetAddress(__CIKCONTEXT_INIT)));
	CBoneAccessor* m_BoneAccessor = pBaseAnimating->GetBoneAccessor();
	CStudioHdr* pStudioHdr = pBaseAnimating->GetModelPtr();
	CIKContext* m_pIk = pBaseAnimating->GetIKContext();

	_ASSERT(m_BoneAccessor);

	if (!pStudioHdr)
	{
		_ASSERT(!"CBaseAnimating::SetupBones pStudioHdr without a model");
		return false;
	}

	if (pStudioHdr->_m_pStudioHdr)
		pStudioHdr->_m_pStudioHdr->eyeposition = Vector(0.0f, 0.0f, 0.0f);;

	if (!m_pIk && pStudioHdr->_m_pStudioHdr->numikchains > 0 && !(*pBaseAnimating->GetEntClientFlagsPtr() & ENTCLIENTFLAG_DONTUSEIK))
	{
		void* pMemory = g_pInterfaces->m_pMemAlloc->Alloc(0x1070);
		if (!pMemory)
		{
			_ASSERT(pMemory);
			return false;
		}
		m_pIk = CreateIK(pMemory);
		*pBaseAnimating->GetIKContextPtr() = m_pIk;
	}

	pBaseAnimating->AddEFlags(EFL_SETTING_UP_BONES);


	BoneVector		pos[MAXSTUDIOBONES];
	BoneQuaternionAligned	q[MAXSTUDIOBONES];

	if (m_pIk)
	{
		if (pBaseAnimating->Teleported() || pBaseAnimating->m_fEffects() & EF_NOINTERP)
		{
			// ClearTargets
			m_pIk->ClearTargets();

		}

		m_pIk->Init(pStudioHdr, (QAngle&)pBaseAnimating->GetAbsAngle(), pBaseAnimating->GetAbsOrigin(), currentTime,g_pInterfaces->m_pGlobalVars->m_nFramecount, boneMask);
		*pBaseAnimating->GetIKContextPtr() = m_pIk;
	}

	int nBoneMask = BONE_USED_BY_HITBOX;
	if (boneMask & BONE_USED_BY_VERTEX_MASK)
		nBoneMask |= BONE_USED_BY_VERTEX_MASK;
	if (boneMask & BONE_USED_BY_BONE_MERGE)
		nBoneMask |= BONE_USED_BY_BONE_MERGE; 


	// GetSkeleton Should Actually Be Here
	pBaseAnimating->StandardBlendingRules(pStudioHdr, pos, q, currentTime, nBoneMask);


	CBoneBitList computed;
	bool DoFancyFeetShuffle = false;


	m_pIk = pBaseAnimating->GetIKContext();
	if (m_pIk && !pBaseAnimating->m_bClientSideRagdoll())
	{

		pBaseAnimating->UpdateIKLocks(currentTime);
		m_pIk->UpdateTargets(pos, q, m_BoneAccessor->GetBoneArrayForWrite(), computed);
		pBaseAnimating->CalculateIKLocks(currentTime);
		m_pIk->SolveDependencies(pos, q, m_BoneAccessor->GetBoneArrayForWrite(), computed);

		{
			if ((pBaseAnimating->IsPlayer() && (boneMask & BONE_USED_BY_VERTEX_LOD0)) || (pBaseAnimating->IsLocalPlayer() && g_pLocalPlayer.m_bRunningFakeAngleBones))
			{
				pBaseAnimating->DoExtraBoneProcessing(pStudioHdr, pos, q, m_BoneAccessor->GetBoneArrayForWrite(), computed, m_pIk);
				DoFancyFeetShuffle = true;
			}
		}
	}

	if (!DoFancyFeetShuffle)
	{
		pStudioHdr->BuildMatrices((const QAngle&)pBaseAnimating->GetAbsAngle(), pBaseAnimating->GetAbsOrigin(), pos, q, -1, 1.0f, m_BoneAccessor->GetBoneArrayForWrite(), boneMask);
	}
	else
	{
		__declspec(align(16)) matrix3x4a_t parentTransform;
		AngleMatrix(pBaseAnimating->GetAbsAngle(), pBaseAnimating->GetAbsOrigin(), parentTransform);
		pBaseAnimating->BuildTransformations(pStudioHdr, pos, q, parentTransform, boneMask, &computed);
	}
	pBaseAnimating->RemoveEFlag(EFL_SETTING_UP_BONES);


#if 1
	pBaseAnimating->ControlMouth(pStudioHdr);

	//if (((boneMask & BONE_USED_BY_ATTACHMENT) || pBaseAnimating->IsLocalPlayer()))
	//	pBaseAnimating->SetupBones_AttachmentHelper(pStudioHdr);
#endif
	// Do they want to get at the bone transforms? If it's just making sure an aiment has 
	// its bones setup, it doesn't need the transforms yet.
	if (pBoneToWorldOut)
	{
		//_ASSERT(!IsEFlagSet(EFL_DIRTY_ABSTRANSFORM), "Cached bone data has old abs origin/angles");
		//AssertMsgOnce( (m_vBoneSetupCachedOrigin == GetRenderOrigin()) && (m_qBoneSetupCachedAngles == GetRenderAngles()), "Renderable moved since cached" );
		if (nMaxBones >= m_BoneAccessor->GetWritableBones())
		{
			Plat_FastMemcpy(pBoneToWorldOut, m_BoneAccessor->GetBoneArrayForWrite(), nMaxBones* m_BoneAccessor->GetWritableBones());
		}
		else
		{
			VCON(XorStr("SetupBones: invalid bone array size (%d - needs %d)\n"), nMaxBones, pBaseAnimating->GetCachedBoneCount());
			return false;
		}
	}

#ifdef MEMORY_DEBUGGING
	_ASSERT(_CrtCheckMemory());
#endif
	return true;
}



// direct reference: [actual address in first opcode] E8 ? ? ? ? 88 44 24 0B 
bool __fastcall hk_CBaseAnimating_ShouldSkipAnimationFrame(void* _this, void*)
{
	PlayerRecord* pRecord = ((Entity*)_this - 4)->GetPlayerRecord();
	if (pRecord && !pRecord->IsInThreadedBonesSetup())
	{
		if (pRecord->LastSetupBones() >= gpGlobals->m_nTickcount)
			return true;
	}

	return false;
}
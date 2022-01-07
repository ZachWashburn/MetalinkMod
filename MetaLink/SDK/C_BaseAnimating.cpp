#include "C_BaseAnimating.h"
#include "../Memory.h"
#include "SDK.h"

CStudioHdr* C_BaseAnimating::GetModelPtr()
{
	// 10568
	return *(CStudioHdr**)((char*)this + *(short*)g_pMemory->GetAddress(__CBASEANIMATINGSTUDIOHDROFFSET));
}


CIKContext* C_BaseAnimating::GetIKContext()
{
	return *(CIKContext**)((char*)this + *(short*)g_pMemory->GetAddress(__CBASEANIMATINGIKCONTEXTOFFSET));
}

CBoneAccessor* C_BaseAnimating::GetBoneAccessor()
{
	return (CBoneAccessor*)((char*)this + (int)9888);//*(CBoneAccessor**)((char*)this + *(short*)g_pMemory->GetAddress(__BONEACCESSOR));
}

CStudioHdr** C_BaseAnimating::GetModelPtrPtr()
{
	// 10568
	return (CStudioHdr**)((char*)this + *(short*)g_pMemory->GetAddress(__CBASEANIMATINGSTUDIOHDROFFSET));
}

void C_BaseAnimating::StandardBlendingRules(CStudioHdr* hdr, Vector* pPos, BoneQuaternionAligned* qa, float curTime, int bonemask)
{
	StandardBlendingRulesFunc_t StandBR = { reinterpret_cast<StandardBlendingRulesFunc_t>(g_pMemory->GetAddress(__STANDARDBLENDINGRULES)) };
	return StandBR(ANIMATINGBASEENTITY, hdr, pPos, qa, curTime, bonemask);
}

bool C_BaseAnimating::IsPlayer() { 
	return GetBaseEntity()->IsPlayer(); 
}

CIKContext** C_BaseAnimating::GetIKContextPtr()
{
	return (CIKContext**)((char*)this + *(short*)g_pMemory->GetAddress(__CBASEANIMATINGIKCONTEXTOFFSET));
}

void C_BaseAnimating::AddEFlags(int nFlags)
{
	*(int*)((char*)this + *(short*)g_pMemory->GetAddress(__CBASEANIMATINGEFLAGS)) |= nFlags;
}
void C_BaseAnimating::RemoveEFlag(int nFlags)
{
	*(int*)((char*)this + *(short*)g_pMemory->GetAddress(__CBASEANIMATINGEFLAGS)) &= ~nFlags;
}

int* C_BaseAnimating::GetEntClientFlagsPtr()
{
	return (int*)((char*)this + *(short*)g_pMemory->GetAddress(__CBASEANIMATINGENTCLIENTFLAGS));
}

CThreadFastMutex* C_BaseAnimating::GetSetupBoneLock()
{
	return (CThreadFastMutex*)((char*)this + *(short*)g_pMemory->GetAddress(__CBASEANIMATINGSETUPBONELOCK));
}

bool C_BaseAnimating::Teleported()
{
	return ((TeleportedFunc_t)g_pMemory->GetAddress(__TELEPORTED))(ANIMATINGBASEENTITY);
}

void C_BaseAnimating::BuildTransformations(CStudioHdr* hdr, BoneVector* pos, BoneQuaternion* q, const matrix3x4_t& cameraTransform, int boneMask, CBoneBitList* boneComputed)
{
	typedef void(__thiscall* BuildTransformationsFunc_t)(C_BaseAnimating*, CStudioHdr*, BoneVector*, BoneQuaternion*, const matrix3x4_t*, int, CBoneBitList*);
	static BuildTransformationsFunc_t BuildTrans{ reinterpret_cast<BuildTransformationsFunc_t>(g_pMemory->GetAddress(__CBASEANIMATIONGBUILDTRANSFORMATIONS)) };
	ZeroMemory(boneComputed, 0x100);	
	BuildTrans(this, hdr, pos, q, &cameraTransform, boneMask, boneComputed);
}

const matrix3x4a_t& C_BaseAnimating::GetBone(int iBone)
{
	return GetBoneAccessor()->GetBone(iBone);
}

CUtlVectorSimple& C_BaseAnimating::GetAttachments()
{
	CUtlVectorSimple* attachments = (CUtlVectorSimple*)((char*)this + *(short*)g_pMemory->GetAddress(__ATTACHMENTS));
	return *attachments;
}

void C_BaseAnimating::FormatViewModelAttachment(int nAttachment, matrix3x4_t* attachmentToWorld)
{
	typedef char(__thiscall* FormatViewModelAttachmentFunc_t)(C_BaseAnimating*, int, matrix3x4_t*);
	static FormatViewModelAttachmentFunc_t FormatViewModel{ reinterpret_cast<FormatViewModelAttachmentFunc_t>(g_pMemory->GetAddress(__CBASEANIMATINGFORMATVIEWMODELATTACHMENT)) };
	FormatViewModel(this, nAttachment, attachmentToWorld);
#if 0
	_asm
	{
		push nAttachment
		push attachmentToWorld
		mov ecx, this
		call FormatViewModel
	}
#endif

}

bool C_BaseAnimating::PutAttachment(int number, const matrix3x4_t& attachmentToWorld)
{
	CUtlVectorSimple& attachments = GetAttachments();
	if (number < 1 || number > attachments.Count())
		return false;

	CAttachmentData* pAtt = (CAttachmentData*)attachments.Retrieve(number - 1, sizeof(CAttachmentData));
	int framecount;

	if (gpGlobals->m_flFrametime <= 0 || (framecount = 2 * pAtt->m_nLastFramecount >> 1, framecount <= 0) || framecount != (gpGlobals->m_nFramecount - 1))
	{
		pAtt->m_vOriginVelocity.Init();
	}
	else
	{
		Vector vecPreviousOrigin, vecOrigin;
		MatrixPosition(pAtt->m_AttachmentToWorld, vecPreviousOrigin);
		MatrixPosition(attachmentToWorld, vecOrigin);
		pAtt->m_vOriginVelocity = (vecOrigin - vecPreviousOrigin) / gpGlobals->m_flFrametime;
	}

	pAtt->m_nLastFramecount ^= (pAtt->m_nLastFramecount ^ gpGlobals->m_nFramecount) & 0x7FFFFFFF;
	pAtt->m_bAnglesComputed &= 0xFE;
	pAtt->m_AttachmentToWorld = attachmentToWorld;

	return true;
}

void C_BaseAnimating::SetupBones_AttachmentHelper(CStudioHdr* hdr, matrix3x4a_t* custommatrix)
{
	// direct reference: [actual address in first opcode] E8 ? ? ? ? 8B 55 08 85 D2 74 23 
	((SetupBones_AttachmentHelperFunc_t)g_pMemory->GetAddress(__SETUPBONES_ATTACHMENTHELPER))(this, hdr);
	return;




	if (!hdr || !hdr->GetNumAttachments())
		return;

	matrix3x4_t world;

	const int numAttachments = hdr->GetNumAttachments();

	for (int i = 0; i < numAttachments; i++)
	{
		mstudioattachment_t& pattachment = hdr->pAttachment(i);
		int iBone = hdr->GetAttachmentBone(i);

		if (pattachment.flags & ATTACHMENT_FLAG_WORLD_ALIGN)
		{
			Vector vecLocalBonePos, vecWorldBonePos;
			MatrixGetColumn(pattachment.local, 3, vecLocalBonePos);;
			VectorTransform(vecLocalBonePos, custommatrix ? custommatrix[iBone] : GetBone(iBone), vecWorldBonePos);

			SetIdentityMatrix(world);
			PositionMatrix(vecWorldBonePos, world);
		}
		else
		{
			ConcatTransforms(custommatrix ? custommatrix[iBone] : GetBone(iBone), pattachment.local, world);
		}
		// VALVE NOTE -> FIXME: this shouldn't be here, it should client side on-demand only and hooked into the bone cache!!
		FormatViewModelAttachment(i, &world);
		PutAttachment(i + 1, world);
	}
}


matrix3x4a_t* C_BaseAnimating::GetCachedBoneBase()
{
	return (matrix3x4a_t*)((char*)this + *(short*)g_pMemory->GetAddress(__CACHEDBONEMATRIXOFFSET));
}

int C_BaseAnimating::GetCachedBoneCount()
{
	return *(int*)((char*)GetCachedBoneBase() + 8);
}

void C_BaseAnimating::BuildTransformationsRebuild(CStudioHdr* hdr, BoneVector* pos, BoneQuaternion* q, const matrix3x4_t& cameraTransform, int boneMask, CBoneBitList& boneComputed)
{
#if 0
	if (!hdr)
		return;

	matrix3x4a_t bonematrix;
	bool boneSimulated[MAXSTUDIOBONES];

	// no bones have been simulated
	memset(boneSimulated, 0, sizeof(boneSimulated));
	const mstudiobone_t* pbones = hdr->pBone(0);
	bool bFixupSimulatedPositions = false;
	if (m_pRagdoll)
	{
		// simulate bones and update flags
		int oldWritableBones = m_BoneAccessor.GetWritableBones();
		int oldReadableBones = m_BoneAccessor.GetReadableBones();
		m_BoneAccessor.SetWritableBones(BONE_USED_BY_ANYTHING);
		m_BoneAccessor.SetReadableBones(BONE_USED_BY_ANYTHING);

		// If we're playing back a demo, override the ragdoll bones with cached version if available - otherwise, simulate.
#if defined( REPLAY_ENABLED )
		if ((!engine->IsPlayingDemo() && !engine->IsPlayingTimeDemo()) ||
			!CReplayRagdollCache::Instance().IsInitialized() ||
			!CReplayRagdollCache::Instance().GetFrame(this, engine->GetDemoPlaybackTick(), boneSimulated, &m_BoneAccessor))
#endif
		{
			m_pRagdoll->RagdollBone(this, pbones, hdr->numbones(), boneSimulated, m_BoneAccessor);
		}

		m_BoneAccessor.SetWritableBones(oldWritableBones);
		m_BoneAccessor.SetReadableBones(oldReadableBones);
		bFixupSimulatedPositions = !m_pRagdoll->GetRagdoll()->allowStretch;
	}

	CalcBoneMerge(boneMask);

	if (CSoftbody* pSoftbody = hdr->GetSoftbody())
	{
		bool bTeleported = (Teleported() || IsEffectActive(EF_NOINTERP));
		pSoftbody->SetAbsOrigin(GetRenderOrigin(), bTeleported);
		pSoftbody->SetAbsAngles(GetRenderAngles(), bTeleported);
		pSoftbody->SetModelScale(GetModelScale());
	}

	for (int i = 0; i < hdr->numbones(); ++i)
	{
		// Only update bones reference by the bone mask.
		if (!(hdr->boneFlags(i) & boneMask))
			continue;

		if (m_pBoneMergeCache && m_pBoneMergeCache->IsBoneMerged(i))
			continue;

		PREFETCH360(&GetBoneForWrite(i), 0);

		// animate all non-simulated bones
		if (boneSimulated[i])
		{
			ApplyBoneMatrixTransform(GetBoneForWrite(i));
			if (bFixupSimulatedPositions && pbones[i].parent != -1)
			{
				Vector boneOrigin;
				VectorTransform(pos[i], GetBone(pbones[i].parent), boneOrigin);
				PositionMatrix(boneOrigin, GetBoneForWrite(i));
			}
			continue;
		}
		else if (CalcProceduralBone(hdr, i, m_BoneAccessor))
		{
			continue;
		}
		// skip bones that the IK has already setup
		else if (boneComputed.IsBoneMarked(i))
		{
			// dummy operation, just used to verify in debug that this should have happened
			GetBoneForWrite(i);
		}
		else
		{
			QuaternionMatrix(q[i], pos[i], bonematrix);

			Assert(fabs(pos[i].x) < 100000);
			Assert(fabs(pos[i].y) < 100000);
			Assert(fabs(pos[i].z) < 100000);

			if ((hdr->boneFlags(i) & BONE_ALWAYS_PROCEDURAL) &&
				(hdr->pBone(i)->proctype & STUDIO_PROC_JIGGLE) &&
				!r_jiggle_bones.GetBool())
			{
				if (m_pJiggleBones)
				{
					delete m_pJiggleBones;
					m_pJiggleBones = NULL;
				}
			}

			if ((hdr->boneFlags(i) & BONE_ALWAYS_PROCEDURAL) &&
				(hdr->pBone(i)->proctype & STUDIO_PROC_JIGGLE) &&
				r_jiggle_bones.GetBool() && m_isJiggleBonesEnabled)
			{
				//
				// Physics-based "jiggle" bone
				// Bone is assumed to be along the Z axis
				// Pitch around X, yaw around Y
				//

				// compute desired bone orientation
				matrix3x4a_t goalMX;

				if (pbones[i].parent == -1)
				{
					ConcatTransforms(cameraTransform, bonematrix, goalMX);
				}
				else
				{
					ConcatTransforms_Aligned(GetBone(pbones[i].parent), bonematrix, goalMX);
				}

				// get jiggle properties from QC data
				mstudiojigglebone_t* jiggleInfo = (mstudiojigglebone_t*)pbones[i].pProcedure();

				if (!m_pJiggleBones)
				{
					m_pJiggleBones = new CJiggleBones;
				}

				// do jiggle physics
				m_pJiggleBones->BuildJiggleTransformations(i, gpGlobals->curtime, jiggleInfo, goalMX, GetBoneForWrite(i), ShouldFlipModel());

			}
			else if (hdr->boneParent(i) == -1)
			{
				ConcatTransforms(cameraTransform, bonematrix, GetBoneForWrite(i));
			}
			else
			{
				ConcatTransforms_Aligned(GetBone(hdr->boneParent(i)), bonematrix, GetBoneForWrite(i));
			}

			if (hdr->boneFlags(i) & BONE_WORLD_ALIGN)
			{
				Vector vecOrigin = GetBone(i).GetOrigin();
				GetBoneForWrite(i).Init(Vector(1, 0, 0), Vector(0, 1, 0), Vector(0, 0, 1), vecOrigin);
			}

		}

		if (hdr->boneParent(i) == -1)
		{
			// Apply client-side effects to the transformation matrix
			ApplyBoneMatrixTransform(GetBoneForWrite(i));
		}
	}

	PostBuildTransformations(hdr, pos, q);

	// If a nonhierarchical scale is being applied
	const float scale = GetModelScale();
	if (GetModelScaleType() == NONHIERARCHICAL_MODEL_SCALE &&
		scale > 1.0f + FLT_EPSILON || scale < 1.0f - FLT_EPSILON)
	{
		for (int i = 0; i < hdr->numbones(); ++i)
		{
			// Only update bones reference by the bone mask.
			if (!(hdr->boneFlags(i) & boneMask))
				continue;

			if (m_pBoneMergeCache && m_pBoneMergeCache->IsBoneMerged(i))
				continue;

			PREFETCH360(&GetBoneForWrite(i), 0);

			matrix3x4_t& transform = GetBoneForWrite(i);

			VectorScale(transform[0], scale, transform[0]);
			VectorScale(transform[1], scale, transform[1]);
			VectorScale(transform[2], scale, transform[2]);
		}
	}

	if (CSoftbody* pSoftbody = hdr->GetSoftbody())
	{
		pSoftbody->GoWakeup();
		matrix3x4a_t* pBones = m_BoneAccessor.GetBoneArrayForWrite();
		pSoftbody->SetAnimatedTransforms(pBones);
		pSoftbody->FilterTransforms(pBones);
	}

	UpdateBoneAttachments();
#endif
}
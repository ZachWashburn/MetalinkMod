#include "CStudioHdr.h"
#include "SDK.h"


void CStudioHdr::BuildMatrices(
	const QAngle& angles,
	const Vector& origin,
	const Vector pos[],
	const Quaternion q[],
	int iBone,
	float flScale,
	matrix3x4_t bonetoworld[MAXSTUDIOBONES],
	int boneMask
)
{
	CStudioHdr* pStudioHdr = this;

	int i, j;

	int					chain[MAXSTUDIOBONES] = {};
	int					chainlength = 0;

	if (iBone < -1 || iBone >= pStudioHdr->numbones())
		iBone = 0;

	// build list of what bones to use
	if (iBone == -1)
	{
		// all bones
		chainlength = pStudioHdr->numbones();
		for (i = 0; i < pStudioHdr->numbones(); i++)
		{
			chain[chainlength - i - 1] = i;
		}
	}
	else
	{
		// only the parent bones
		i = iBone;
		while (i != -1)
		{
			chain[chainlength++] = i;
			i = pStudioHdr->boneParent(i);
		}
	}

	matrix3x4_t bonematrix;
	matrix3x4_t rotationmatrix; // model to world transformation
	AngleMatrix(angles, origin, rotationmatrix);

	// Account for a change in scale
	if (flScale < 1.0f - FLT_EPSILON || flScale > 1.0f + FLT_EPSILON)
	{
		Vector vecOffset;
		MatrixGetColumn(rotationmatrix, 3, vecOffset);
		vecOffset -= origin;
		vecOffset *= flScale;
		vecOffset += origin;
		MatrixSetColumn(vecOffset, 3, rotationmatrix);

		// Scale it uniformly
		VectorScale(rotationmatrix[0], flScale, rotationmatrix[0]);
		VectorScale(rotationmatrix[1], flScale, rotationmatrix[1]);
		VectorScale(rotationmatrix[2], flScale, rotationmatrix[2]);
	}

	for (j = chainlength - 1; j >= 0; j--)
	{
		i = chain[j];
		if (pStudioHdr->boneFlags(i) & boneMask)
		{
			QuaternionMatrix(q[i], pos[i], bonematrix);

			if (pStudioHdr->boneParent(i) == -1)
			{
				ConcatTransforms(rotationmatrix, bonematrix, bonetoworld[i]);
			}
			else
			{
				ConcatTransforms(bonetoworld[pStudioHdr->boneParent(i)], bonematrix, bonetoworld[i]);
			}
		}
	}
}

int CStudioHdr::GetNumAttachments()
{
	virtualmodel_t* v1; // eax
	int result; // eax

	v1 = this->m_pVModel;
	if (v1)
		result = v1->m_attachment.count;
	else
		result = this->_m_pStudioHdr->numlocalattachments;
	return result;
}


mstudioattachment_t& CStudioHdr::pAttachment(int i)
{
	CStudioHdr* hdr; // edi
	virtualmodel_t* vmodel; // ecx
	const studiohdr_t* pStudioHdr; // eax

	hdr = this;
	vmodel = this->m_pVModel;
	if (!vmodel)
		return *(mstudioattachment_t*)((char*)hdr->_m_pStudioHdr + ((92 * i) + hdr->_m_pStudioHdr->localattachmentindex));
	pStudioHdr = (const studiohdr_t*)GroupStudioHdr(*(DWORD*)(vmodel->m_attachment.memory + 8 * i));
	//return *(mstudioattachment_t *)((char *)pStudioHdr
	//	+ 92 * *(DWORD *)(hdr->m_pVModel->m_attachment.memory + 8 * i + 4)
	//	+ pStudioHdr->localattachmentindex);

	DWORD v11 = 92 * *(DWORD*)(vmodel->m_attachment.memory + 8 * i + 4);
	DWORD v10 = pStudioHdr->localattachmentindex + v11;
	return *(mstudioattachment_t*)((char*)pStudioHdr + v10);
}

int CStudioHdr::GetAttachmentBone(int i)
{
	virtualmodel_t* vmodel = m_pVModel;
	int iBone;

	if (vmodel)
	{
		DWORD v14 = vmodel->m_group.memory + 144 * *(DWORD*)(vmodel->m_attachment.memory + 8 * i);
		iBone = *(DWORD*)(*(DWORD*)(v14 + 24) + 4 * pAttachment(i).localbone);
		if (iBone == -1)
			iBone = 0;
	}
	else
	{
		iBone = *(int*)((char*)&_m_pStudioHdr->checksum + (92 * i) + _m_pStudioHdr->localattachmentindex);
	}
	return iBone;
}

#include "../../Memory.h"
const studiohdr_t* CStudioHdr::GroupStudioHdr(DWORD group)
{
	typedef const studiohdr_t* (__thiscall*GroupStudioHdrFunc_t)(CStudioHdr*, DWORD);
	static GroupStudioHdrFunc_t oGroupStudioHdr{ reinterpret_cast<GroupStudioHdrFunc_t>(g_pMemory->GetAddress(__GROUPSTUDIOHDR)) };
	return oGroupStudioHdr(this, group);
}
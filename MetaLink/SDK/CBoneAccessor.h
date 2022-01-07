//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef BONE_ACCESSOR_H
#define BONE_ACCESSOR_H
#ifdef _WIN32
#pragma once
#endif
#include "../BaseIncludes.h"
#include "mathlib/mathlib.h"

class CStudioHdr;
class C_BaseAnimating;


class CBoneAccessor
{
public:

	CBoneAccessor();
	CBoneAccessor(matrix3x4a_t *pBones); // This can be used to allow access to all bones.


	void Init(const C_BaseAnimating *pAnimating, matrix3x4a_t *pBones);

	int GetReadableBones();
	void SetReadableBones(int flags);

	void SetBoneToWorld(matrix3x4a_t* pMat) { m_pBones = pMat; };

	int GetWritableBones();
	void SetWritableBones(int flags);

	// Get bones for read or write access.
	const matrix3x4a_t&	GetBone(int iBone) const;
	const matrix3x4a_t&	operator[](int iBone) const;
	matrix3x4a_t&		GetBoneForWrite(int iBone);


	void SetBoneArrayForWrite(matrix3x4a_t* p_bones);
	inline matrix3x4a_t* GetBoneArrayForWrite(void) const
	{
		return m_pBones;
	}


	void* m_pAnimating;
	matrix3x4a_t* m_pBones;
	int        m_ReadableBones;
	int        m_WritableBones;
};


inline CBoneAccessor::CBoneAccessor()
{
	m_pAnimating = NULL;
	m_pBones = NULL;
	m_ReadableBones = m_WritableBones = 0;
}

inline CBoneAccessor::CBoneAccessor(matrix3x4a_t *pBones)
{
	m_pAnimating = NULL;
	m_pBones = pBones;
}

inline void CBoneAccessor::Init(const C_BaseAnimating *pAnimating, matrix3x4a_t *pBones)
{
	m_pAnimating = (void*)pAnimating;
	m_pBones = pBones;
}

inline int CBoneAccessor::GetReadableBones()
{
	return m_ReadableBones;
}

inline void CBoneAccessor::SetReadableBones(int flags)
{
	m_ReadableBones = flags;
}

inline int CBoneAccessor::GetWritableBones()
{
	return m_WritableBones;
}

inline void CBoneAccessor::SetWritableBones(int flags)
{
	m_WritableBones = flags;
}

inline const matrix3x4a_t& CBoneAccessor::GetBone(int iBone) const
{
	return m_pBones[iBone];
}

inline const matrix3x4a_t& CBoneAccessor::operator[](int iBone) const
{
	return m_pBones[iBone];
}

inline matrix3x4a_t& CBoneAccessor::GetBoneForWrite(int iBone)
{
	return m_pBones[iBone];
}


inline void CBoneAccessor::SetBoneArrayForWrite(matrix3x4a_t* p_bones)
{
	m_pBones = p_bones;
}
#endif
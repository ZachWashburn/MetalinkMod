#include "CIKContext.h"
#include "SDK.h"
#include "../Memory.h"

void BuildBoneChainPartial(
	const CStudioHdr* pStudioHdr,
	const matrix3x4_t& rootxform,
	const BoneVector pos[],
	const BoneQuaternion q[],
	int	iBone,
	matrix3x4_t* pBoneToWorld,
	CBoneBitList& boneComputed,
	int iRoot);


class CIKSolver
{
public:
	//-------- SOLVE TWO LINK INVERSE KINEMATICS -------------
	// Author: Ken Perlin
	//
	// Given a two link joint from [0,0,0] to end effector position P,
	// let link lengths be a and b, and let norm |P| = c.  Clearly a+b <= c.
	//
	// Problem: find a "knee" position Q such that |Q| = a and |P-Q| = b.
	//
	// In the case of a point on the x axis R = [c,0,0], there is a
	// closed form solution S = [d,e,0], where |S| = a and |R-S| = b:
	//
	//    d2+e2 = a2                  -- because |S| = a
	//    (c-d)2+e2 = b2              -- because |R-S| = b
	//
	//    c2-2cd+d2+e2 = b2           -- combine the two equations
	//    c2-2cd = b2 - a2
	//    c-2d = (b2-a2)/c
	//    d - c/2 = (a2-b2)/c / 2
	//
	//    d = (c + (a2-b2/c) / 2      -- to solve for d and e.
	//    e = sqrt(a2-d2)

	static float findD(float a, float b, float c) {
		return (c + (a * a - b * b) / c) / 2;
	}
	static float findE(float a, float d) { return sqrt(a * a - d * d); }

	// This leads to a solution to the more general problem:
	//
	//   (1) R = Mfwd(P)         -- rotate P onto the x axis
	//   (2) Solve for S
	//   (3) Q = Minv(S)         -- rotate back again

	float Mfwd[3][3];
	float Minv[3][3];

	bool solve(float A, float B, float const P[], float const D[], float Q[]) {
		float R[3];
		defineM(P, D);
		rot(Minv, P, R);
		float r = length(R);
		float d = findD(A, B, r);
		float e = findE(A, d);
		float S[3] = { d,e,0 };
		rot(Mfwd, S, Q);
		return d > (r - B) && d < A;
	}

	// If "knee" position Q needs to be as close as possible to some point D,
	// then choose M such that M(D) is in the y>0 half of the z=0 plane.
	//
	// Given that constraint, define the forward and inverse of M as follows:

	void defineM(float const P[], float const D[]) {
		float* X = Minv[0], * Y = Minv[1], * Z = Minv[2];

		// Minv defines a coordinate system whose x axis contains P, so X = unit(P).
		int i;
		for (i = 0; i < 3; i++)
			X[i] = P[i];
		normalize(X);

		// Its y axis is perpendicular to P, so Y = unit( E - X(E·X) ).

		float dDOTx = dot(D, X);
		for (i = 0; i < 3; i++)
			Y[i] = D[i] - dDOTx * X[i];
		normalize(Y);

		// Its z axis is perpendicular to both X and Y, so Z = X×Y.

		cross(X, Y, Z);

		// Mfwd = (Minv)T, since transposing inverts a rotation matrix.

		for (i = 0; i < 3; i++) {
			Mfwd[i][0] = Minv[0][i];
			Mfwd[i][1] = Minv[1][i];
			Mfwd[i][2] = Minv[2][i];
		}
	}

	//------------ GENERAL VECTOR MATH SUPPORT -----------

	static float dot(float const a[], float const b[]) { return a[0] * b[0] + a[1] * b[1] + a[2] * b[2]; }

	static float length(float const v[]) { return sqrt(dot(v, v)); }

	static void normalize(float v[]) {
		float norm = length(v);
		for (int i = 0; i < 3; i++)
			v[i] /= norm;
	}

	static void cross(float const a[], float const b[], float c[]) {
		c[0] = a[1] * b[2] - a[2] * b[1];
		c[1] = a[2] * b[0] - a[0] * b[2];
		c[2] = a[0] * b[1] - a[1] * b[0];
	}

	static void rot(float const M[3][3], float const src[], float dst[]) {
		for (int i = 0; i < 3; i++)
			dst[i] = dot(M[i], src);
	}
};

void BuildBoneChain(
	const CStudioHdr* pStudioHdr,
	const matrix3x4a_t& rootxform,
	const BoneVector pos[],
	const BoneQuaternion q[],
	int	iBone,
	matrix3x4a_t* pBoneToWorld)
{
	CBoneBitList boneComputed;
	BuildBoneChainPartial(pStudioHdr, rootxform, pos, q, iBone, pBoneToWorld, boneComputed, -1);
	return;
}
//-----------------------------------------------------------------------------
// Purpose: visual debugging code
//-----------------------------------------------------------------------------
#if 1
inline void debugLine(const Vector& origin, const Vector& dest, int r, int g, int b, bool noDepthTest, float duration) { };
#else
extern void drawLine(const Vector& p1, const Vector& p2, int r = 0, int g = 0, int b = 1, bool noDepthTest = true, float duration = 0.1);
void debugLine(const Vector& origin, const Vector& dest, int r, int g, int b, bool noDepthTest, float duration)
{
	drawLine(origin, dest, r, g, b, noDepthTest, duration);
}
#endif


//-----------------------------------------------------------------------------
// Purpose: for a 2 bone chain, find the IK solution and reset the matrices
//-----------------------------------------------------------------------------
bool Studio_SolveIK(mstudioikchain_t* pikchain, Vector& targetFoot, matrix3x4a_t* pBoneToWorld)
{
#if 0
	// FIXME: something with the CS models breaks this, why?
	if (pikchain->pLink(0)->kneeDir.LengthSqr() > 0.0)
	{
		Vector targetKneeDir, targetKneePos;
		// FIXME: knee length should be as long as the legs
		Vector tmp = pikchain->pLink(0)->kneeDir;
		VectorRotate(tmp, pBoneToWorld[pikchain->pLink(0)->bone], targetKneeDir);
		MatrixPosition(pBoneToWorld[pikchain->pLink(1)->bone], targetKneePos);
		return Studio_SolveIK(pikchain->pLink(0)->bone, pikchain->pLink(1)->bone, pikchain->pLink(2)->bone, targetFoot, targetKneePos, targetKneeDir, pBoneToWorld);
	}
	else
#endif
	{
		return Studio_SolveIK(pikchain->pLink(0)->bone, pikchain->pLink(1)->bone, pikchain->pLink(2)->bone, targetFoot, pBoneToWorld);
	}
}


#define KNEEMAX_EPSILON 0.9998 // (0.9998 is about 1 degree)

//-----------------------------------------------------------------------------
// Purpose: Solve Knee position for a known hip and foot location, but no specific knee direction preference
//-----------------------------------------------------------------------------

bool Studio_SolveIK(int iThigh, int iKnee, int iFoot, Vector& targetFoot, matrix3x4a_t* pBoneToWorld)
{

	Vector worldFoot, worldKnee, worldThigh;

	MatrixPosition(pBoneToWorld[iThigh], worldThigh);
	MatrixPosition(pBoneToWorld[iKnee], worldKnee);
	MatrixPosition(pBoneToWorld[iFoot], worldFoot);

	//debugLine( worldThigh, worldKnee, 0, 0, 255, true, 0 );
	//debugLine( worldKnee, worldFoot, 0, 0, 255, true, 0 );

	Vector ikFoot, ikKnee;

	ikFoot = targetFoot - worldThigh;
	ikKnee = worldKnee - worldThigh;

	float l1 = (worldKnee - worldThigh).Length();
	float l2 = (worldFoot - worldKnee).Length();
	float l3 = (worldFoot - worldThigh).Length();

	// leg too straight to figure out knee?
	if (l3 > (l1 + l2) * KNEEMAX_EPSILON)
	{
		return false;
	}

	// If any of the thigh to knee to foot bones are co-positional, then solving ik doesn't make sense. 
	// We're probably looking at uninitialized bones or something
	if (l1 <= 0 || l2 <= 0 || l3 <= 0)
	{
		return false;
	}

	Vector ikHalf = (worldFoot - worldThigh) * (l1 / l3);

	// FIXME: what to do when the knee completely straight?
	Vector ikKneeDir = ikKnee - ikHalf;
	VectorNormalize(ikKneeDir);

	return Studio_SolveIK(iThigh, iKnee, iFoot, targetFoot, worldKnee, ikKneeDir, pBoneToWorld);
}

//-----------------------------------------------------------------------------
// Purpose: Realign the matrix so that its X axis points along the desired axis.
//-----------------------------------------------------------------------------
void Studio_AlignIKMatrix(matrix3x4a_t& mMat, const Vector& vAlignTo)
{

	Vector tmp1, tmp2, tmp3;

	// Column 0 (X) becomes the vector.
	tmp1 = vAlignTo;
	VectorNormalize(tmp1);
	MatrixSetColumn(tmp1, 0, mMat);

	// Column 1 (Y) is the cross of the vector and column 2 (Z).
	MatrixGetColumn(mMat, 2, tmp3);
	tmp2 = tmp3.Cross(tmp1);
	VectorNormalize(tmp2);
	// FIXME: check for X being too near to Z
	MatrixSetColumn(tmp2, 1, mMat);

	// Column 2 (Z) is the cross of columns 0 (X) and 1 (Y).
	tmp3 = tmp1.Cross(tmp2);
	MatrixSetColumn(tmp3, 2, mMat);
}


//-----------------------------------------------------------------------------
// Purpose: Solve Knee position for a known hip and foot location, and a known knee direction
//-----------------------------------------------------------------------------

bool Studio_SolveIK(int iThigh, int iKnee, int iFoot, Vector& targetFoot, Vector& targetKneePos, Vector& targetKneeDir, matrix3x4a_t* pBoneToWorld)
{

	Vector worldFoot, worldKnee, worldThigh;

	MatrixPosition(pBoneToWorld[iThigh], worldThigh);
	MatrixPosition(pBoneToWorld[iKnee], worldKnee);
	MatrixPosition(pBoneToWorld[iFoot], worldFoot);

	//debugLine( worldThigh, worldKnee, 0, 0, 255, true, 0 );
	//debugLine( worldThigh, worldThigh + targetKneeDir, 0, 0, 255, true, 0 );
	// debugLine( worldKnee, targetKnee, 0, 0, 255, true, 0 );

	Vector ikFoot, ikTargetKnee, ikKnee;

	ikFoot = targetFoot - worldThigh;
	ikKnee = targetKneePos - worldThigh;

	float l1 = (worldKnee - worldThigh).Length();
	float l2 = (worldFoot - worldKnee).Length();

	// exaggerate knee targets for legs that are nearly straight
	// FIXME: should be configurable, and the ikKnee should be from the original animation, not modifed
	float d = (targetFoot - worldThigh).Length() - MIN(l1, l2);
	d = MAX(l1 + l2, d);
	// FIXME: too short knee directions cause trouble
	d = d * 100;

	ikTargetKnee = ikKnee + targetKneeDir * d;

	// debugLine( worldKnee, worldThigh + ikTargetKnee, 0, 0, 255, true, 0 );

	int color[3] = { 0, 255, 0 };

	// too far away? (0.9998 is about 1 degree)
	if (ikFoot.Length() > (l1 + l2) * KNEEMAX_EPSILON)
	{
		VectorNormalize(ikFoot);
		VectorScale(ikFoot, (l1 + l2) * KNEEMAX_EPSILON, ikFoot);
		color[0] = 255; color[1] = 0; color[2] = 0;
	}

	// too close?
	// limit distance to about an 80 degree knee bend
	float minDist = MAX(fabs(l1 - l2) * 1.15, MIN(l1, l2) * 0.15);
	if (ikFoot.Length() < minDist)
	{
		// too close to get an accurate vector, just use original vector
		ikFoot = (worldFoot - worldThigh);
		VectorNormalize(ikFoot);
		VectorScale(ikFoot, minDist, ikFoot);
	}

	CIKSolver ik;
	if (ik.solve(l1, l2, ikFoot.Base(), ikTargetKnee.Base(), ikKnee.Base()))
	{
		matrix3x4a_t& mWorldThigh = pBoneToWorld[iThigh];
		matrix3x4a_t& mWorldKnee = pBoneToWorld[iKnee];
		matrix3x4a_t& mWorldFoot = pBoneToWorld[iFoot];

		//debugLine( worldThigh, ikKnee + worldThigh, 255, 0, 0, true, 0 );
		//debugLine( ikKnee + worldThigh, ikFoot + worldThigh, 255, 0, 0, true,0 );

		// debugLine( worldThigh, ikKnee + worldThigh, color[0], color[1], color[2], true, 0 );
		// debugLine( ikKnee + worldThigh, ikFoot + worldThigh, color[0], color[1], color[2], true,0 );


		// build transformation matrix for thigh
		Studio_AlignIKMatrix(mWorldThigh, ikKnee);
		Studio_AlignIKMatrix(mWorldKnee, ikFoot - ikKnee);


		mWorldKnee[0][3] = ikKnee.x + worldThigh.x;
		mWorldKnee[1][3] = ikKnee.y + worldThigh.y;
		mWorldKnee[2][3] = ikKnee.z + worldThigh.z;

		mWorldFoot[0][3] = ikFoot.x + worldThigh.x;
		mWorldFoot[1][3] = ikFoot.y + worldThigh.y;
		mWorldFoot[2][3] = ikFoot.z + worldThigh.z;

		return true;
	}
	else
	{
		/*
		debugLine( worldThigh, worldThigh + ikKnee, 255, 0, 0, true, 0 );
		debugLine( worldThigh + ikKnee, worldThigh + ikFoot, 255, 0, 0, true, 0 );
		debugLine( worldThigh + ikFoot, worldThigh, 255, 0, 0, true, 0 );
		debugLine( worldThigh + ikKnee, worldThigh + ikTargetKnee, 255, 0, 0, true, 0 );
		*/
		return false;
	}
}

//------

//-----------------------------------------------------------------------------
// Purpose: build boneToWorld transforms for a specific bone
//-----------------------------------------------------------------------------
void BuildBoneChain(
	const CStudioHdr* pStudioHdr,
	const matrix3x4a_t& rootxform,
	const BoneVector pos[],
	const BoneQuaternion q[],
	int	iBone,
	matrix3x4a_t* pBoneToWorld,
	CBoneBitList& boneComputed)
{
	BuildBoneChainPartial(pStudioHdr, rootxform, pos, q, iBone, pBoneToWorld, boneComputed, -1);
}


void BuildBoneChainPartial(
	const CStudioHdr* pStudioHdr,
	const matrix3x4_t& rootxform,
	const BoneVector pos[],
	const BoneQuaternion q[],
	int	iBone,
	matrix3x4_t* pBoneToWorld,
	CBoneBitList& boneComputed,
	int iRoot)
{
	if (boneComputed.IsBoneMarked(iBone))
		return;

	matrix3x4_t bonematrix;
	QuaternionMatrix(q[iBone], pos[iBone], bonematrix);

	int parent = pStudioHdr->boneParent(iBone);
	if (parent == -1 || iBone == iRoot)
	{
		ConcatTransforms(rootxform, bonematrix, pBoneToWorld[iBone]);
	}
	else
	{
		// evil recursive!!!
		BuildBoneChainPartial(pStudioHdr, rootxform, pos, q, parent, pBoneToWorld, boneComputed, iRoot);
		ConcatTransforms(pBoneToWorld[parent], bonematrix, pBoneToWorld[iBone]);
	}

	boneComputed.MarkBone(iBone);
}


void CIKContext::UpdateTargets(BoneVector pos[], BoneQuaternion q[], matrix3x4a_t boneToWorld[], CBoneBitList& boneComputed)
{
	static CIKContextUpdateTargetsFunc_t UpdateTargets(reinterpret_cast<CIKContextUpdateTargetsFunc_t>(g_pMemory->GetAddress(__CIKCONTEXTUPDATETARGETS)));
	UpdateTargets(this, pos, q, boneToWorld, std::ref(boneComputed));
}



void CIKContext::ClearTargets()
{

	int i;
	for (i = 0; i < m_target.Count(); i++)
	{
		m_target[i].latched.iFramecounter = -9999;
	}

	return;
#if 0
	// ClearTargets
	DWORD i = 0;
	DWORD MaxLoops = *(DWORD*)((char*)this + (int)4080);
	if (MaxLoops > 0)
	{
		DWORD* v63 = (DWORD*)((char*)this + (int)208);
		do
		{
			*v63 = -9999;
			v63 += 85;
			++i;
		} while (i < MaxLoops);
	}
#endif
}

void CIKContext::Init(CStudioHdr* pStudioHdr, QAngle& angles, Vector& pos, float flTime, int iFramecounter, int boneMask)
{
	//static CIKConextInitFunc_t InitIK(reinterpret_cast<CIKConextInitFunc_t>(g_pMemory->GetAddress(__CIKCONTEXT_INIT)));
	//InitIK(this, pStudioHdr, angles, pos, flTime, iFramecounter, boneMask);
	//return;
	m_pStudioHdr = pStudioHdr;
	m_ikChainRule.RemoveAll(); // m_numikrules = 0;
	if (pStudioHdr->numikchains())
	{
		m_ikChainRule.SetSize(pStudioHdr->numikchains());

		// FIXME: Brutal hackery to prevent a crash
		if (m_target.Count() == 0)
		{
			m_target.SetSize(12);
			memset(m_target.Base(), 0, sizeof(m_target[0]) * m_target.Count());
			ClearTargets();
		}

	}
	else
	{
		m_target.SetSize(0);
	}
	AngleMatrix(angles, pos, m_rootxform);
	m_iFramecounter = iFramecounter;
	m_flTime = flTime;
	m_boneMask = boneMask;


#if 0
	m_pStudioHdr = pStudioHdr;
	AngleMatrix(angles, pos, m_rootxform);
	m_iFramecounter = iFramecounter;
	m_flTime = flTime;
	m_boneMask = boneMask;
#endif
}

void CIKContext::BuildBoneChain(
	const BoneVector pos[],
	const BoneQuaternion q[],
	int	iBone,
	matrix3x4a_t* pBoneToWorld,
	CBoneBitList& boneComputed)
{
	_ASSERT(m_pStudioHdr->boneFlags(iBone) & m_boneMask);
	::BuildBoneChain(m_pStudioHdr, m_rootxform, pos, q, iBone, pBoneToWorld, boneComputed);
}


void SolveBone(
	const CStudioHdr* pStudioHdr,
	int	iBone,
	matrix3x4a_t* pBoneToWorld,
	BoneVector pos[],
	BoneQuaternion q[]
)
{
	int iParent = pStudioHdr->boneParent(iBone);

	matrix3x4a_t worldToBone;
	MatrixInvert(pBoneToWorld[iParent], worldToBone);

	matrix3x4a_t local;
	ConcatTransforms_Aligned(worldToBone, pBoneToWorld[iBone], local);

	MatrixAngles(local, q[iBone], pos[iBone]);
}

void CIKContext::SolveDependencies(BoneVector* pos, BoneQuaternion* q, matrix3x4a_t* boneToWorld, CBoneBitList& boneComputed)
{
	//static CIKContextSolveDependenciesFunc_t SolveDepends(reinterpret_cast<CIKContextSolveDependenciesFunc_t>(g_pMemory->GetAddress(__CIKCONTEXTSOLVEDEPENDENCIES)));
	//SolveDepends(this, pos, q, boneToWorld, boneComputed);
	//return;
#if 1
	matrix3x4a_t worldTarget;
	int i, j;

	ikchainresult_t chainResult[32]; // allocate!!!

	// init chain rules
	for (i = 0; i < m_pStudioHdr->numikchains(); i++)
	{
		mstudioikchain_t* pchain = m_pStudioHdr->pIKChain(i);
		ikchainresult_t* pChainResult = &chainResult[i];
		int bone = pchain->pLink(2)->bone;

		pChainResult->target = -1;
		pChainResult->flWeight = 0.0f;

		// don't bother with chain if the bone isn't going to be calculated
		if (!(m_pStudioHdr->boneFlags(bone) & m_boneMask))
			continue;

		// eval current ik'd bone
		BuildBoneChain(pos, q, bone, boneToWorld, std::ref(boneComputed));

		MatrixAngles(boneToWorld[bone], pChainResult->q, pChainResult->pos);
	}

	for (j = 0; j < m_ikChainRule.Count(); j++)
	{
		for (i = 0; i < m_ikChainRule.Element(j).Count(); i++)
		{
			ikcontextikrule_t* pRule = &m_ikChainRule.Element(j).Element(i);
			ikchainresult_t* pChainResult = &chainResult[pRule->chain];
			pChainResult->target = -1;


			switch (pRule->type)
			{
			case IK_SELF:
			{
				// xform IK target error into world space
				matrix3x4a_t local;
				QuaternionMatrix(pRule->q, pRule->pos, local);
				// eval target bone space
				if (pRule->bone != -1)
				{
					BuildBoneChain(pos, q, pRule->bone, boneToWorld, std::ref(boneComputed));
					ConcatTransforms_Aligned(boneToWorld[pRule->bone], local, worldTarget);
				}
				else
				{
					ConcatTransforms_Aligned(m_rootxform, local, worldTarget);
				}

				float flWeight = pRule->flWeight * pRule->flRuleWeight;
				pChainResult->flWeight = pChainResult->flWeight * (1.0f - flWeight) + flWeight;

				Vector p2;
				Quaternion q2;

				// target p and q
				MatrixAngles(worldTarget, q2, p2);

				// debugLine( pChainResult->pos, p2, 0, 0, 255, true, 0.1 );

				// blend in position and angles
				pChainResult->pos = pChainResult->pos * (1.0f - flWeight) + p2 * flWeight;
				QuaternionSlerp(pChainResult->q, q2, flWeight, pChainResult->q);
			}
			break;
			case IK_WORLD:
				Assert(0);
				break;

			case IK_ATTACHMENT:
				break;

			case IK_GROUND:
				break;

			case IK_RELEASE:
			{
				// move target back towards original location
				float flWeight = pRule->flWeight * pRule->flRuleWeight;
				mstudioikchain_t* pchain = m_pStudioHdr->pIKChain(pRule->chain);
				int bone = pchain->pLink(2)->bone;

				Vector p2;
				Quaternion q2;

				BuildBoneChain(pos, q, bone, boneToWorld, std::ref(boneComputed));
				MatrixAngles(boneToWorld[bone], q2, p2);

				// blend in position and angles
				pChainResult->pos = pChainResult->pos * (1.0 - flWeight) + p2 * flWeight;
				QuaternionSlerp(pChainResult->q, q2, flWeight, pChainResult->q);
			}
			break;
			case IK_UNLATCH:
			{
				/*
				pChainResult->flWeight = pChainResult->flWeight * (1 - pRule->flWeight) + pRule->flWeight;
				pChainResult->pos = pChainResult->pos * (1.0 - pRule->flWeight ) + pChainResult->local.pos * pRule->flWeight;
				QuaternionSlerp( pChainResult->q, pChainResult->local.q, pRule->flWeight, pChainResult->q );
				*/
			}
			break;
}
		}
	}

	for (i = 0; i < m_target.Count(); i++)
	{
		CIKTarget* pTarget = &m_target[i];

		if (m_target[i].est.flWeight > 0.0)
		{
			matrix3x4a_t worldFootpad;
			matrix3x4a_t local;
			//mstudioikchain_t *pchain = m_pStudioHdr->pIKChain( m_target[i].chain );
			ikchainresult_t* pChainResult = &chainResult[pTarget->chain];

			AngleMatrix(RadianEuler(pTarget->offset.q), pTarget->offset.pos, local);

			AngleMatrix(RadianEuler(pTarget->est.q), pTarget->est.pos, worldFootpad);

			ConcatTransforms_Aligned(worldFootpad, local, worldTarget);

			Vector p2;
			Quaternion q2;
			// target p and q
			MatrixAngles(worldTarget, q2, p2);
			// MatrixAngles( worldTarget, pChainResult->q, pChainResult->pos );

			// blend in position and angles
			pChainResult->flWeight = pTarget->est.flWeight;
			pChainResult->pos = pChainResult->pos * (1.0 - pChainResult->flWeight) + p2 * pChainResult->flWeight;
			QuaternionSlerp(pChainResult->q, q2, pChainResult->flWeight, pChainResult->q);
		}

		if (pTarget->latched.bNeedsLatch)
		{
			// keep track of latch position
			pTarget->latched.bHasLatch = true;
			pTarget->latched.q = pTarget->est.q;
			pTarget->latched.pos = pTarget->est.pos;
		}
	}

	for (i = 0; i < m_pStudioHdr->numikchains(); i++)
	{
		ikchainresult_t* pChainResult = &chainResult[i];
		mstudioikchain_t* pchain = m_pStudioHdr->pIKChain(i);

		if (pChainResult->flWeight > 0.0)
		{
			Vector tmp;
			MatrixPosition(boneToWorld[pchain->pLink(2)->bone], tmp);
			// debugLine( pChainResult->pos, tmp, 255, 255, 255, true, 0.1 );

			// do exact IK solution
			// FIXME: once per link!
			if (Studio_SolveIK(pchain, pChainResult->pos, boneToWorld))
			{
				Vector p3;
				MatrixGetColumn(boneToWorld[pchain->pLink(2)->bone], 3, p3);
				QuaternionMatrix(pChainResult->q, p3, boneToWorld[pchain->pLink(2)->bone]);

				// rebuild chain
				// FIXME: is this needed if everyone past this uses the boneToWorld array?
				SolveBone(m_pStudioHdr, pchain->pLink(2)->bone, boneToWorld, pos, q);
				SolveBone(m_pStudioHdr, pchain->pLink(1)->bone, boneToWorld, pos, q);
				SolveBone(m_pStudioHdr, pchain->pLink(0)->bone, boneToWorld, pos, q);
			}
			else
			{
				// FIXME: need to invalidate the targets that forced this...
				if (pChainResult->target != -1)
				{
					CIKTarget* pTarget = &m_target[pChainResult->target];
					VectorScale(pTarget->latched.deltaPos, 0.8, pTarget->latched.deltaPos);
					QuaternionScale(pTarget->latched.deltaQ, 0.8, pTarget->latched.deltaQ);
				}
			}
		}
	}
#endif
#if 0
	Vector p1, p2, p3;
	Quaternion q1, q2, q3;

	// current p and q
	MatrixAngles(boneToWorld[bone], q1, p1);


	// target p and q
	MatrixAngles(worldTarget, q2, p2);

	// blend in position and angles
	p3 = p1 * (1.0 - m_ikRule[i].flWeight) + p2 * m_ikRule[i].flWeight;

	// do exact IK solution
	// FIXME: once per link!
	Studio_SolveIK(pchain, p3, boneToWorld);

	// force angle (bad?)
	QuaternionSlerp(q1, q2, m_ikRule[i].flWeight, q3);
	MatrixGetColumn(boneToWorld[bone], 3, p3);
	QuaternionMatrix(q3, p3, boneToWorld[bone]);

	// rebuild chain
	SolveBone(m_pStudioHdr, pchain->pLink(2)->bone, boneToWorld, pos, q);
	SolveBone(m_pStudioHdr, pchain->pLink(1)->bone, boneToWorld, pos, q);
	SolveBone(m_pStudioHdr, pchain->pLink(0)->bone, boneToWorld, pos, q);
#endif

}

#if 0
int __thiscall m_pIK_Init(void* this, int a2, int a3, int a4, int a5, int a6, int a7)
{
	int v7; // eax
	_DWORD* v8; // edi
	char* v9; // ecx
	int v10; // esi
	_DWORD* v11; // edi
	int v12; // ebx
	int v13; // eax
	int v14; // ecx
	_DWORD* v15; // eax
	int result; // eax
	_DWORD* v17; // [esp+8h] [ebp-8h]
	char* v18; // [esp+Ch] [ebp-4h]

	v7 = a2;
	v8 = this;
	v9 = (char*)this + 4092;
	v17 = v8;
	v18 = v9;
	v8[1022] = a2;
	v10 = *((_DWORD*)v9 + 3) - 1;
	if (v10 >= 0)
	{
		v11 = v9;
		v12 = 20 * v10;
		do
		{
			sub_1018DBC0(v12 + *v11);
			v12 -= 20;
			--v10;
		} while (v10 >= 0);
		v8 = v17;
		v9 = v18;
		v7 = a2;
	}
	*((_DWORD*)v9 + 3) = 0;
	v13 = *(_DWORD*)(*(_DWORD*)v7 + 284);
	if (v13)
	{
		sub_10797EA0(v13);
		if (!v8[1020])
		{
			v8[1020] = 0;
			sub_10797F70(0, 12);
			memset(v8, 0, 340 * v8[1020]);
			v14 = 0;
			if (v8[1020] > 0)
			{
				v15 = v8 + 52;
				do
				{
					*v15 = -9999;
					v15 += 85;
					++v14;
				} while (v14 < v8[1020]);
			}
		}
	}
	else
	{
		v8[1020] = 0;
		sub_10797F70(0, 0);
	}
	sub_10813EC0((float*)a3, (int)(v8 + 1036));
	v8[1039] = *(_DWORD*)a4;
	v8[1043] = *(_DWORD*)(a4 + 4);
	v8[1047] = *(_DWORD*)(a4 + 8);
	v8[1048] = a6;
	result = a7;
	v8[1049] = a5;
	v8[1050] = a7;
	return result;
}
#endif
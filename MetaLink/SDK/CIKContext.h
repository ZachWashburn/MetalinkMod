#pragma once
#include "../BaseIncludes.h"
#include "CStudioHdr.h"
#include "mathlib/mathlib.h"
#include "../FunctionTypeDeclares.h"
#include "bitvec.h"


//inline CIKContextConstructorFunc_t g_CreateIKContext;
struct ikchainresult_t;
struct ikcontextikrule_t;
class CIKTarget;
class CBoneBitList;

class CBoneBitList : public CBitVec<MAXSTUDIOBONES>
{
public:
	inline void MarkBone(int iBone)
	{
		Set(iBone);
	}
	inline bool IsBoneMarked(int iBone)
	{
		return Get(iBone) != 0 ? true : false;
	}
};


class CIKTarget
{
public:
#if 0
	void SetOwner(int entindex, const Vector& pos, const QAngle& angles);
	void ClearOwner(void);
	int GetOwner(void);
	void UpdateOwner(int entindex, const Vector& pos, const QAngle& angles);
	void SetPos(const Vector& pos);
	void SetAngles(const QAngle& angles);
	void SetQuaternion(const Quaternion& q);
	void SetNormal(const Vector& normal);
	void SetPosWithNormalOffset(const Vector& pos, const Vector& normal);
	void SetOnWorld(bool bOnWorld = true);

	bool IsActive(void);
	void IKFailed(void);
#endif
	int chain;
	int type;
	//void MoveReferenceFrame(Vector& deltaPos, QAngle& deltaAngles);
	// accumulated offset from ideal footplant location
public:
	struct x2 {
		char* pAttachmentName;
		Vector		pos;
		Quaternion	q;
	} offset;
private:
	struct x3 {
		Vector		pos;
		Quaternion	q;
	} ideal;
public:
	struct x4 {
		float		latched;
		float		release;
		float		height;
		float		floor;
		float		radius;
		float		flTime;
		float		flWeight;
		Vector		pos;
		Quaternion	q;
		bool		onWorld;
	} est; // estimate contact position
	struct x5 {
		float		hipToFoot;	// distance from hip
		float		hipToKnee;	// distance from hip to knee
		float		kneeToFoot;	// distance from knee to foot
		Vector		hip;		// location of hip
		Vector		closest;	// closest valid location from hip to foot that the foot can move to
		Vector		knee;		// pre-ik location of knee
		Vector		farthest;	// farthest valid location from hip to foot that the foot can move to
		Vector		lowest;		// lowest position directly below hip that the foot can drop to
	} trace;
private:
	// internally latched footset, position
	struct x1 {
		// matrix3x4a_t		worldTarget;
		bool		bNeedsLatch;
		bool		bHasLatch;
		float		influence;
		int			iFramecounter;
		int			owner;
		Vector		absOrigin;
		QAngle		absAngles;
		Vector		pos;
		Quaternion	q;
		Vector		deltaPos;	// acculated error
		Quaternion	deltaQ;
		Vector		debouncePos;
		Quaternion	debounceQ;
	} latched;
	struct x6 {
		float		flTime; // time last error was detected
		float		flErrorTime;
		float		ramp;
		bool		bInError;
	} error;

	friend class CIKContext;
};



class CIKContext 
{
public:
	void Init(CStudioHdr* pStudioHdr, QAngle& angles, Vector& pos, float flTime, int iFramecounter, int boneMask);
	void UpdateTargets(BoneVector *pos, BoneQuaternion *q, matrix3x4a_t *boneToWorld, CBoneBitList& boneComputed);
	void SolveDependencies(BoneVector *pos, BoneQuaternion *q, matrix3x4a_t *boneToWorld, CBoneBitList& boneComputed);
	void ClearTargets(void);
	const int GetBoneMask(void) { return m_boneMask; }
	void BuildBoneChain(const BoneVector pos[], const BoneQuaternion q[], int iBone, matrix3x4a_t* pBoneToWorld, CBoneBitList& boneComputed);
#if 0
	CIKContext( );
	void Init( const CStudioHdr *pStudioHdr, const QAngle &angles, const Vector &pos, float flTime, int iFramecounter, int boneMask );
	void AddDependencies(  mstudioseqdesc_t &seqdesc, int iSequence, float flCycle, const float poseParameters[], float flWeight = 1.0f );

#if defined( _PS3 )
	void AddAllDependencies_PS3( ikcontextikrule_t *ikRules, int numRules );
#endif


	void UpdateTargets( BoneVector pos[], BoneQuaternion q[], matrix3x4a_t boneToWorld[], CBoneBitList &boneComputed );
	void AutoIKRelease( void );
	void SolveDependencies( BoneVector pos[], BoneQuaternion q[], matrix3x4a_t boneToWorld[], CBoneBitList &boneComputed );

	void AddAutoplayLocks( BoneVector pos[], BoneQuaternion q[] );
	void SolveAutoplayLocks( BoneVector pos[], BoneQuaternion q[] );

	void AddSequenceLocks( mstudioseqdesc_t &SeqDesc, BoneVector pos[], BoneQuaternion q[] );
	void SolveSequenceLocks( mstudioseqdesc_t &SeqDesc, BoneVector pos[], 	BoneQuaternion q[] );
	
	void AddAllLocks( BoneVector pos[], BoneQuaternion q[] );
	void SolveAllLocks( BoneVector pos[], BoneQuaternion q[] );

	void SolveLock( const mstudioiklock_t *plock, int i, BoneVector pos[], BoneQuaternion q[], matrix3x4a_t boneToWorld[], CBoneBitList &boneComputed );



	void CopyTo( CIKContext* pOther, const unsigned short * iRemapping );




#endif
private:
	CUtlVectorFixed< CIKTarget, 12 >	m_target;
	CStudioHdr const* m_pStudioHdr;
#if 1
	CUtlVector< CUtlVector< ikcontextikrule_t > > m_ikChainRule;
	CUtlVector< ikcontextikrule_t > m_ikLock;
	matrix3x4a_t m_rootxform;

	int m_iFramecounter;
	float m_flTime;
	int m_boneMask;
#endif
};




/* THESE PROBABLY ARE WRONG */
struct ikchainresult_t
{
	// accumulated offset from ideal footplant location
	int			target;
	Vector		pos;
	Quaternion	q;
	float		flWeight;
};



struct ikcontextikrule_t
{
	int			index;

	int			type;
	int			chain;

	int			bone;

	int			slot;	// iktarget slot.  Usually same as chain.
	float		height;
	float		radius;
	float		floor;
	Vector		pos;
	float		pad1;
	Quaternion	q;

	float		start;	// beginning of influence
	float		peak;	// start of full influence
	float		tail;	// end of full influence
	float		end;	// end of all influence

	float		top;
	float		drop;

	float		commit;		// frame footstep target should be committed
	float		release;	// frame ankle should end rotation from latched orientation

	float		flWeight;		// processed version of start-end cycle
	float		flRuleWeight;	// blending weight
	float		latched;		// does the IK rule use a latched value?
	char* szLabel;

	Vector		kneeDir;
	float		pad2;
	Vector		kneePos;
	float		pad3;

	ikcontextikrule_t() {}

private:
	// No copy constructors allowed
	ikcontextikrule_t(const ikcontextikrule_t& vOther);
};

bool Studio_SolveIK(mstudioikchain_t* pikchain, Vector& targetFoot, matrix3x4a_t* pBoneToWorld);
bool Studio_SolveIK(int iThigh, int iKnee, int iFoot, Vector& targetFoot, matrix3x4a_t* pBoneToWorld);
void Studio_AlignIKMatrix(matrix3x4a_t& mMat, const Vector& vAlignTo);
bool Studio_SolveIK(int iThigh, int iKnee, int iFoot, Vector& targetFoot, Vector& targetKneePos, Vector& targetKneeDir, matrix3x4a_t* pBoneToWorld);
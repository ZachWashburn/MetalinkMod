#pragma once
#include "../BaseIncludes.h"
#include "tier1/utlvector.h"
#include "tier1/utlhash.h"
#include "tier1/utlsymbol.h"
#include "mathlib/mathlib.h"
#include "Studio.h"




class CStudioHdr
{
public:
	void BuildMatrices(
		const QAngle& angles,
		const Vector& origin,
		const Vector pos[],
		const Quaternion q[],
		int iBone,
		float flScale,
		matrix3x4_t bonetoworld[MAXSTUDIOBONES],
		int boneMask
	);



	mutable studiohdr_t* _m_pStudioHdr; //0
	mutable virtualmodel_t* m_pVModel; //4
	mutable void* m_pSoftbody; //8
	CUtlVectorSimple m_pStudioHdrCache; //20
	int m_nFrameUnlockCounter; //32
	int* m_pFrameUnlockCounter; //36
	CThreadFastMutex m_FrameUnlockCounterMutex; //40
	CUtlVector<int>		m_boneFlags; //48
	CUtlVector<int>		m_boneParent; //68

		// This class maps an activity to sequences allowed for that activity, accelerating the resolution
	// of SelectWeightedSequence(), especially on PowerPC. Iterating through every sequence
	// attached to a model turned out to be a very destructive cache access pattern on 360.
	// 
	// I've encapsulated this behavior inside a nested class for organizational reasons; there is
	// no particular programmatic or efficiency benefit to it. It just makes clearer what particular
	// code in the otherwise very complicated StudioHdr class has to do with this particular
	// optimization, and it lets you collapse the whole definition down to a single line in Visual
	// Studio.
		class CActivityToSequenceMapping /* final */
		{
		public:
			// A tuple of a sequence and its corresponding weight. Lists of these correspond to activities.
			struct SequenceTuple
			{
				short		seqnum;
				short		weight; // the absolute value of the weight from the sequence header
				CUtlSymbol* pActivityModifiers;		// list of activity modifier symbols
				int			iNumActivityModifiers;
				int* iUnknown;
			};

			// The type of the hash's stored data, a composite of both key and value
			// (because that's how CUtlHash works):
			// key: an int, the activity #
			// values: an index into the m_pSequenceTuples array, a count of the
			// total sequences present for an activity, and the sum of their
			// weights.
			// Note this struct is 128-bits wide, exactly coincident to a PowerPC 
			// cache line and VMX register. Please consider very carefully the
			// performance implications before adding any additional fields to this.
			// You could probably do away with totalWeight if you really had to.
			struct HashValueType
			{
				// KEY (hashed)
				int activityIdx;

				// VALUE (not hashed)
				int startingIdx;
				int count;
				int totalWeight;

				HashValueType(int _actIdx, int _stIdx, int _ct, int _tW) :
					activityIdx(_actIdx), startingIdx(_stIdx), count(_ct), totalWeight(_tW) {}

				// default constructor (ought not to be actually used)
				HashValueType() : activityIdx(-1), startingIdx(-1), count(-1), totalWeight(-1)
				{
					//AssertMsg(false, "Don't use default HashValueType()!");
				}


				class HashFuncs
				{
				public:
					// dummy constructor (gndn)
					HashFuncs(int) {}

					// COMPARE
					// compare two entries for uniqueness. We should never have two different
					// entries for the same activity, so we only compare the activity index;
					// this allows us to use the utlhash as a dict by constructing dummy entries
					// as hash lookup keys.
					bool operator()(const HashValueType& lhs, const HashValueType& rhs) const
					{
						return lhs.activityIdx == rhs.activityIdx;
					}

					// HASH
					// We only hash on the activity index; everything else is data.
					unsigned int operator()(const HashValueType& item) const
					{
						return HashInt(item.activityIdx);
					}
				};
			};

			typedef CUtlHash<HashValueType, HashValueType::HashFuncs, HashValueType::HashFuncs> ActivityToValueIdxHash;

			// These must be here because IFM does not compile/link studio.cpp (?!?)

			// ctor
			CActivityToSequenceMapping(void)
				: m_pSequenceTuples(NULL), m_iSequenceTuplesCount(0), m_ActToSeqHash(8, 0, 0), m_pStudioHdr(NULL), m_expectedVModel(NULL)
			{};

			// dtor -- not virtual because this class has no inheritors
			~CActivityToSequenceMapping()
			{
				if (m_pSequenceTuples != NULL)
				{
					if (m_pSequenceTuples->pActivityModifiers != NULL)
					{
						delete[] m_pSequenceTuples->pActivityModifiers;
					}
					delete[] m_pSequenceTuples;
				}
			}

			/// Get the list of sequences for an activity. Returns the pointer to the
			/// first sequence tuple. Output parameters are a count of sequences present,
			/// and the total weight of all the sequences. (it would be more LHS-friendly
			/// to return these on registers, if only C++ offered more than one return 
			/// value....)
			const SequenceTuple* GetSequences(int forActivity, int* outSequenceCount, int* outTotalWeight);

			/// The number of sequences available for an activity.
			int NumSequencesForActivity(int forActivity);

			static CActivityToSequenceMapping* FindMapping(const CStudioHdr* pstudiohdr);
			static void ReleaseMapping(CActivityToSequenceMapping* pMap);
			static void ResetMappings();

			//private:

				/// Allocate my internal array. (It is freed in the destructor.) Also,
				/// build the hash of activities to sequences and populate m_pSequenceTuples.
			void Initialize(const CStudioHdr* pstudiohdr);

			/// Force Initialize() to occur again, even if it has already occured.
			void Reinitialize(CStudioHdr* pstudiohdr);

			/// A more efficient version of the old SelectWeightedSequence() function in animation.cpp. 
			int SelectWeightedSequence(CStudioHdr* pstudiohdr, int activity, int curSequence);

			// selects the sequence with the most matching modifiers
			int SelectWeightedSequenceFromModifiers(CStudioHdr* pstudiohdr, int activity, CUtlSymbol* pActivityModifiers, int iModifierCount, CBaseEntity* parent);

			// Actually a big array, into which the hash values index.
			SequenceTuple* m_pSequenceTuples;
			unsigned int m_iSequenceTuplesCount; // (size of the whole array)

			// we don't store an outer pointer because we can't initialize it at construction time
			// (warning c4355) -- there are ways around this but it's easier to just pass in a 
			// pointer to the CStudioHdr when we need it, since this class isn't supposed to 
			// export its interface outside the studio header anyway.
			// CStudioHdr * const m_pOuter;

			ActivityToValueIdxHash m_ActToSeqHash;

			const studiohdr_t* m_pStudioHdr;

			// we store these so we can know if the contents of the studiohdr have changed
			// from underneath our feet (this is an emergency data integrity check)
			const void* m_expectedVModel;


			// double-check that the data I point to hasn't changed
			bool ValidateAgainst(const CStudioHdr* __restrict pstudiohdr);
			void SetValidation(const CStudioHdr* __restrict pstudiohdr);

			friend class CStudioHdr;
		};

		CActivityToSequenceMapping* m_pActivityToSequence; //88

		mstudioattachment_t& pAttachment(int i);
		int GetNumAttachments();
		const studiohdr_t* GroupStudioHdr(DWORD group);
		int GetAttachmentBone(int i);
		inline int boneFlags(int iBone) const { return m_boneFlags[iBone]; }
		inline int boneParent(int iBone) const { return m_boneParent[iBone]; }
		inline int			numbones(void) const { return _m_pStudioHdr->numbones; };
		inline mstudiobone_t* pBone(int i) const { return _m_pStudioHdr->pBone(i); };
		//there is more, screw the rest for now..

		inline bool IsValid(void) { return (_m_pStudioHdr != NULL); };
		inline bool IsReadyForAccess(void) const { return (_m_pStudioHdr != NULL); };
		inline const studiohdr_t* GetRenderHdr(void) const { return _m_pStudioHdr; };
		int	GetNumPoseParameters(void) const;
		const mstudioposeparamdesc_t& pPoseParameter(int i);
		inline int			numikchains() const { return _m_pStudioHdr->numikchains; };
		inline int			GetNumIKChains(void) const { return _m_pStudioHdr->numikchains; };
		inline mstudioikchain_t* pIKChain(int i) const { return _m_pStudioHdr->pIKChain(i); };
		mstudioseqdesc_t& pSeqdesc(int seq);
		virtualmodel_t* GetVirtualModel() const { return m_pVModel; }
		inline const byte* GetBoneTableSortedByName() const { return _m_pStudioHdr->GetBoneTableSortedByName(); };
		bool SequencesAvailable();
		int GetNumSeq(void) const;
		int GetActivityListVersion();

};

#pragma pack(push, 1)
class CAttachmentData
{
public:
	matrix3x4_t m_AttachmentToWorld;
	QAngle m_angRotation;
	Vector m_vOriginVelocity; 
	int m_nLastFramecount;
	unsigned char m_bAnglesComputed;
	char pad[3];
};
#pragma pack(pop)

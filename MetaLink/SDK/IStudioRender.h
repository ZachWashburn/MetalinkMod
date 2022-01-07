#pragma once
#include "../BaseIncludes.h"
#include "IMaterialSystem.h"
#include "vtf.h"
#include "Studio.h"
#include "IStudioRender.h"
// undone: what's the standard for function type naming?
typedef void (*StudioRender_Printf_t)( PRINTF_FORMAT_STRING const char *fmt, ... );

struct StudioRenderConfig_t
{
	float fEyeShiftX;	// eye X position
	float fEyeShiftY;	// eye Y position
	float fEyeShiftZ;	// eye Z position
	float fEyeSize;		// adjustment to iris textures
	float fEyeGlintPixelWidthLODThreshold;

	int maxDecalsPerModel;
	int drawEntities;
	int skin;
	int fullbright;

	bool bEyeMove : 1;		// look around
	bool bSoftwareSkin : 1;
	bool bNoHardware : 1;
	bool bNoSoftware : 1;
	bool bTeeth : 1;
	bool bEyes : 1;
	bool bFlex : 1;
	bool bWireframe : 1;
	bool bDrawNormals : 1;
	bool bDrawTangentFrame : 1;
	bool bDrawZBufferedWireframe : 1;
	bool bSoftwareLighting : 1;
	bool bShowEnvCubemapOnly : 1;
	bool bWireframeDecals : 1;

	// Reserved for future use
	int m_nReserved[4];
};



//-----------------------------------------------------------------------------
// Studio render interface
//-----------------------------------------------------------------------------

#define STUDIORENDER_DECAL_INVALID  ( (StudioDecalHandle_t)0 )

enum
{
	ADDDECAL_TO_ALL_LODS = -1
};


//-----------------------------------------------------------------------------
// DrawModel flags
//-----------------------------------------------------------------------------
enum
{
	STUDIORENDER_DRAW_ENTIRE_MODEL		= 0,
	STUDIORENDER_DRAW_OPAQUE_ONLY		= 0x01,
	STUDIORENDER_DRAW_TRANSLUCENT_ONLY	= 0x02,
	STUDIORENDER_DRAW_GROUP_MASK		= 0x03,
	STUDIORENDER_DRAW_NO_FLEXES			= 0x04,
	STUDIORENDER_DRAW_STATIC_LIGHTING	= 0x08,
	STUDIORENDER_DRAW_ACCURATETIME		= 0x10,		// Use accurate timing when drawing the model.
	STUDIORENDER_DRAW_NO_SHADOWS		= 0x20,
	STUDIORENDER_DRAW_GET_PERF_STATS	= 0x40,
	STUDIORENDER_DRAW_WIREFRAME			= 0x80,
	STUDIORENDER_DRAW_ITEM_BLINK		= 0x100,
	STUDIORENDER_SHADOWDEPTHTEXTURE		= 0x200,								
	STUDIORENDER_UNUSED					= 0x400,
	STUDIORENDER_SKIP_DECALS			= 0x800,
	STUDIORENDER_MODEL_IS_CACHEABLE     = 0x1000,
	STUDIORENDER_SHADOWDEPTHTEXTURE_INCLUDE_TRANSLUCENT_MATERIALS = 0x2000,
	STUDIORENDER_NO_PRIMARY_DRAW		= 0x4000,
	STUDIORENDER_SSAODEPTHTEXTURE		= 0x8000,
};


//-----------------------------------------------------------------------------
// Standard model vertex formats
//-----------------------------------------------------------------------------
// FIXME: remove these (materials/shaders should drive vertex format). Need to
//        list required forcedmaterialoverrides in models/bsps (rather than
//        all models supporting all possible overrides, as they do currently).
#define VERTEX_TEXCOORD0_2D ( ( (uint64) 2 ) << ( TEX_COORD_SIZE_BIT + ( 3*0 ) ) )
enum MaterialVertexFormat_t
{
	//MATERIAL_VERTEX_FORMAT_MODEL	= (VertexFormat_t) VERTEX_POSITION | VERTEX_COLOR_STREAM_1 | VERTEX_NORMAL | VERTEX_TEXCOORD0_2D | VERTEX_USERDATA_SIZE(4),
};


//-----------------------------------------------------------------------------
// What kind of material override is it?
//-----------------------------------------------------------------------------
enum OverrideType_t
{
	OVERRIDE_NORMAL = 0,
	OVERRIDE_BUILD_SHADOWS,
	OVERRIDE_DEPTH_WRITE,
	OVERRIDE_SELECTIVE,
	OVERRIDE_SSAO_DEPTH_WRITE,
};


//-----------------------------------------------------------------------------
// DrawModel info
//-----------------------------------------------------------------------------

// Special flag for studio models that have a compiled in shadow lod version
// It's negative 2 since positive numbers == use a regular slot and -1 means
//  have studiorender compute a value instead
enum
{
	USESHADOWLOD = -2,
};

// beyond this number of materials, you won't get info back from DrawModel
#define MAX_DRAW_MODEL_INFO_MATERIALS 8

struct DrawModelResults_t
{
	int m_ActualTriCount; 
	int m_TextureMemoryBytes;
	int m_NumHardwareBones;
	int m_NumBatches;
	int m_NumMaterials;
	int m_nLODUsed;
	int m_flLODMetric;
	//CFastTimer m_RenderTime;
	//CUtlVectorFixed<IMaterial *,MAX_DRAW_MODEL_INFO_MATERIALS> m_Materials;
};

struct ColorMeshInfo_t
{
	// A given color mesh can own a unique Mesh, or it can use a shared Mesh
	// (in which case it uses a sub-range defined by m_nVertOffset and m_nNumVerts)
	IMesh				*	m_pMesh;
	void	*	m_pPooledVBAllocator;
	int						m_nVertOffsetInBytes;
	int						m_nNumVerts;
};

struct DrawModelInfo_t
{
	studiohdr_t* m_pStudioHdr;
	void* m_pHardwareData;
	void* m_Decals;
	int				m_Skin;
	int				m_Body;
	int				m_HitboxSet;
	void* m_pClientEntity;
	int				m_Lod;
	ColorMeshInfo_t* m_pColorMeshes;
	bool			m_bStaticLighting;
	uint32_t	m_LightingState;
};

enum
{
	// This is because we store which flashlights are on which model 
	// in a 32-bit field (see ModelArrayInstanceData_t::m_nFlashlightUsage)
	MAX_FLASHLIGHTS_PER_INSTANCE_DRAW_CALL = 32
};

struct FlashlightInstance_t
{
	IMaterial *m_pDebugMaterial;
	FlashlightState_t m_FlashlightState;
	VMatrix m_WorldToTexture;
	ITexture *m_pFlashlightDepthTexture;
};

struct StudioModelArrayInfo2_t
{
	int						m_nFlashlightCount;
	FlashlightInstance_t	*m_pFlashlights;	// NOTE: Can have at most MAX_FLASHLIGHTS_PER_INSTANCE_DRAW_CALL of these
};

struct StudioModelArrayInfo_t  : public StudioModelArrayInfo2_t
{
	studiohdr_t				*m_pStudioHdr;
	uint32_t			*m_pHardwareData;
};

struct StudioArrayData_t
{
	studiohdr_t				*m_pStudioHdr;
	uint32_t*m_pHardwareData;
	void					*m_pInstanceData; // See StudioShadowArrayInstanceData_t or StudioArrayInstanceData_t
	int						m_nCount;
};

struct StudioShadowArrayInstanceData_t 
{
	int m_nLOD;
	int m_nBody;
	int m_nSkin;
	matrix3x4a_t *m_pPoseToWorld;
	float *m_pFlexWeights;
	float *m_pDelayedFlexWeights;
};

struct StudioArrayInstanceData_t : public StudioShadowArrayInstanceData_t 
{
	MaterialLightingState_t *m_pLightingState;
	MaterialLightingState_t *m_pDecalLightingState;
	ITexture *m_pEnvCubemapTexture;
	uint32_t m_Decals;
	uint32 m_nFlashlightUsage;	// Mask indicating which flashlights to use.
	ShaderStencilState_t *m_pStencilState;
	ColorMeshInfo_t *m_pColorMeshInfo;
	bool m_bColorMeshHasIndirectLightingOnly;
	Vector4D m_DiffuseModulation;
};

struct GetTriangles_Vertex_t
{
	Vector m_Position;
	Vector m_Normal;
	Vector4D m_TangentS;
	Vector2D m_TexCoord;
	Vector4D m_BoneWeight;
	int m_BoneIndex[4];
	int m_NumBones;

	IMPLEMENT_OPERATOR_EQUAL( GetTriangles_Vertex_t );
};

struct GetTriangles_MaterialBatch_t
{
	IMaterial *m_pMaterial;
	CUtlVector<GetTriangles_Vertex_t> m_Verts;
	CUtlVector<int> m_TriListIndices;
};

struct GetTriangles_Output_t
{
	CUtlVector<GetTriangles_MaterialBatch_t> m_MaterialBatches;
	matrix3x4_t m_PoseToWorld[MAXSTUDIOBONES];

	DISALLOW_OPERATOR_EQUAL( GetTriangles_Output_t );
};

//-----------------------------------------------------------------------------
// Cache Callback Function
// implementation can either statically persist data (tools) or lru cache (engine) it.
// caller returns base pointer to resident data.
// code expectes data to be dynamic and invokes cache callback prior to iterative access.
// virtualModel is member passed in via studiohdr_t and passed back for model identification.
//-----------------------------------------------------------------------------
#define STUDIO_DATA_CACHE_INTERFACE_VERSION "VStudioDataCache005"
 
abstract_class IStudioDataCache : public IAppSystem
{
public:
	virtual bool VerifyHeaders( studiohdr_t *pStudioHdr ) = 0;
	virtual uint32_t*CacheVertexData( studiohdr_t *pStudioHdr ) = 0;
};



abstract_class IStudioRender : public IAppSystem
{
public:
	SUBFUNC(); // virtual void BeginFrame( void ) = 0;
	SUBFUNC(); // virtual void EndFrame( void ) = 0;
	SUBFUNC(); // virtual void Mat_Stub( IMaterialSystem *pMatSys ) = 0;
	SUBFUNC(); // virtual void UpdateConfig( const StudioRenderConfig_t& config ) = 0;
	SUBFUNC(); // virtual void GetCurrentConfig( StudioRenderConfig_t& config ) = 0;
	SUBFUNC(); // virtual bool LoadModel( studiohdr_t *pStudioHdr, void *pVtxData, uint32_t*pHardwareData ) = 0;
	SUBFUNC(); // virtual void UnloadModel(uint32_t*pHardwareData ) = 0;
	SUBFUNC(); // virtual void RefreshStudioHdr( studiohdr_t* pStudioHdr, uint32_t* pHardwareData ) = 0;
	SUBFUNC(); // virtual void SetEyeViewTarget( const studiohdr_t *pStudioHdr, int nBodyIndex, const Vector& worldPosition ) = 0;
	SUBFUNC(); // virtual int GetNumAmbientLightSamples() = 0;
	SUBFUNC(); // virtual const Vector *GetAmbientLightDirections() = 0;
	SUBFUNC(); // virtual void SetAmbientLightColors( const Vector4D *pAmbientOnlyColors ) = 0;
	SUBFUNC(); // virtual void SetAmbientLightColors( const Vector *pAmbientOnlyColors ) = 0;
	SUBFUNC(); // virtual void SetLocalLights( int numLights, const LightDesc_t *pLights ) = 0;
	SUBFUNC(); // virtual void SetViewState( const Vector& viewOrigin, const Vector& viewRight, const Vector& viewUp, const Vector& viewPlaneNormal ) = 0;
	SUBFUNC(); // virtual int GetNumLODs( const uint32_t& hardwareData ) const = 0;
	SUBFUNC(); // virtual float GetLODSwitchValue( const uint32_t& hardwareData, int lod ) const = 0;
	SUBFUNC(); // virtual void SetLODSwitchValue(uint32_t &hardwareData, int lod, float switchValue ) = 0;
	SUBFUNC(); // virtual void SetColorModulation( float const* pColor ) = 0;
	SUBFUNC(); // virtual void SetAlphaModulation( float flAlpha ) = 0;
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	virtual void DrawModel( DrawModelResults_t *pResults, const DrawModelInfo_t& info, matrix3x4_t *pBoneToWorld, float *pFlexWeights, float *pFlexDelayedWeights, const Vector &modelOrigin, int flags = STUDIORENDER_DRAW_ENTIRE_MODEL ) = 0;
	virtual void DrawModelStaticProp( const DrawModelInfo_t& drawInfo, const matrix3x4_t &modelToWorld, int flags = STUDIORENDER_DRAW_ENTIRE_MODEL ) = 0;
	virtual void DrawStaticPropDecals( const DrawModelInfo_t &drawInfo, const matrix3x4_t &modelToWorld ) = 0;
	virtual void DrawStaticPropShadows( const DrawModelInfo_t &drawInfo, const matrix3x4_t &modelToWorld, int flags ) = 0;
	virtual void ForcedMaterialOverride( IMaterial *newMaterial, OverrideType_t nOverrideType = OVERRIDE_NORMAL, int nMaterialIndex = -1 ) = 0;
	virtual bool IsForcedMaterialOverride() = 0;
	virtual uint32_t CreateDecalList(uint32_t*pHardwareData ) = 0;
	virtual void DestroyDecalList(uint32_t handle ) = 0;
	virtual void AddDecal(uint32_t handle, studiohdr_t *pStudioHdr, matrix3x4_t *pBoneToWorld,const uint32_t& ray, const Vector& decalUp, IMaterial* pDecalMaterial, float radius, int body, bool noPokethru = false, int maxLODToDecal = ADDDECAL_TO_ALL_LODS, void *pvProxyUserData = NULL, int nAdditionalDecalFlags = 0 ) = 0;
	virtual void ComputeLighting( const Vector* pAmbient, int lightCount,LightDesc_t* pLights, const Vector& pt, const Vector& normal, Vector& lighting ) = 0;
	virtual void ComputeLightingConstDirectional( const Vector* pAmbient, int lightCount,LightDesc_t* pLights, const Vector& pt, const Vector& normal, Vector& lighting, float flDirectionalAmount ) = 0;
	virtual void AddShadow( IMaterial* pMaterial, void* pProxyData, FlashlightState_t *m_pFlashlightState = NULL, VMatrix *pWorldToTexture = NULL, ITexture *pFlashlightDepthTexture = NULL ) = 0;
	virtual void ClearAllShadows() = 0;
	virtual int ComputeModelLod(uint32_t* pHardwareData, float unitSphereSize, float *pMetric = NULL ) = 0;
	virtual void GetPerfStats( DrawModelResults_t *pResults, const DrawModelInfo_t &info, CUtlBuffer *pSpewBuf = NULL ) const = 0;
	virtual void GetTriangles( const DrawModelInfo_t& info, matrix3x4_t *pBoneToWorld, GetTriangles_Output_t &out ) = 0;
	virtual int GetMaterialList( studiohdr_t *pStudioHdr, int count, IMaterial** ppMaterials ) = 0;
	virtual int GetMaterialListFromBodyAndSkin( MDLHandle_t studio, int nSkin, int nBody, int nCountOutputMaterials, IMaterial** ppOutputMaterials ) = 0;
	virtual void DrawModelArrayStaticProp( const DrawModelInfo_t& drawInfo, int nInstanceCount, const MeshInstanceData_t *pInstanceData, ColorMeshInfo_t **pColorMeshes ) = 0;
	virtual void DrawModelArray( const StudioModelArrayInfo_t &drawInfo, int nCount, StudioArrayInstanceData_t *pInstanceData, int nInstanceStride, int flags = STUDIORENDER_DRAW_ENTIRE_MODEL ) = 0;
	virtual void DrawModelShadowArray( int nCount, StudioArrayData_t *pShadowData,int nInstanceStride, int flags = STUDIORENDER_DRAW_ENTIRE_MODEL ) = 0;
	virtual void DrawModelArray( const StudioModelArrayInfo2_t &info, int nCount, StudioArrayData_t *pArrayData, int nInstanceStride, int flags = STUDIORENDER_DRAW_ENTIRE_MODEL ) = 0;
#ifndef _CERT
	struct FacesRenderedInfo_t
	{
		studiohdr_t *pStudioHdr;
		unsigned int nFaceCount;
		unsigned int nRenderCount;
	};

	typedef void ( * FaceInfoCallbackFunc_t )( int nTopN, FacesRenderedInfo_t *pRenderedFaceInfo, int nTotalFaces );
	virtual void GatherRenderedFaceInfo(FaceInfoCallbackFunc_t pFunc) = 0;
#endif // !_CERT
};

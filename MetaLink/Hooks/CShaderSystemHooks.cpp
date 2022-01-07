#include "CShaderSystemHooks.h"
#include "../UI/GoldenRenderer.h"
enum
{
	// The flags up here are computed from the shaders themselves
	// lighting flags
	SHADER_UNLIT					= 0x0000,
	SHADER_VERTEX_LIT				= 0x0001,
	SHADER_NEEDS_LIGHTMAP			= 0x0002,
	SHADER_NEEDS_BUMPED_LIGHTMAPS	= 0x0004,
	SHADER_LIGHTING_MASK			= 0x0007,


// opacity flags
	SHADER_OPACITY_ALPHATEST = 0x0010,
	SHADER_OPACITY_OPAQUE = 0x0020,
	SHADER_OPACITY_TRANSLUCENT = 0x0040,
	SHADER_OPACITY_MASK = 0x0070,
};

enum
{
	MAX_RENDER_PASSES = 3
};


//-----------------------------------------------------------------------------
// Information for a single render pass
//-----------------------------------------------------------------------------
struct RenderPassList_t
{
	int m_nPassCount;
#if 0
	StateSnapshot_t	m_Snapshot[MAX_RENDER_PASSES];
	// per material shader-defined state
	CBasePerMaterialContextData* m_pContextData[MAX_RENDER_PASSES];
	CBasePerInstanceContextData* m_pInstanceData[MAX_RENDER_PASSES];
#endif
};

struct ShaderRenderState_t
{
	// These are the same, regardless of whether alpha or color mod is used
	int				m_Flags;	// Can't shrink this to a short
#if 0
	VertexFormat_t	m_VertexFormat;
	VertexFormat_t	m_VertexUsage;

	// List of all snapshots
	RenderPassList_t* m_pSnapshots;
#endif
};


//-----------------------------------------------------------------------------
// Used to get the snapshot count
//-----------------------------------------------------------------------------
enum
{
	SNAPSHOT_COUNT_NORMAL = 8,
	SNAPSHOT_COUNT_EDITOR = 16,
	SNAPSHOT_COUNT_GBUFFER = 64,
};

typedef void(__thiscall* CShaderSystemDrawElementsFunc_t)(void* _this, IShader* pShader,
	IMaterialVar** params,
	ShaderRenderState_t* pRenderState,
	VertexCompressionType_t vertexCompression,
	uint32 nMaterialVarChangeTimeStamp,
	uint32 nModulationFlags,
	bool bRenderingPreTessPatchMesh);
void __fastcall hk_CShaderSystem_DrawElements(void* _this, void*, IShader* pShader,
	IMaterialVar** params,
	ShaderRenderState_t* pRenderState,
	VertexCompressionType_t vertexCompression,
	uint32 nMaterialVarChangeTimeStamp,
	uint32 nModulationFlags,
	bool bRenderingPreTessPatchMesh)
{


	((CShaderSystemDrawElementsFunc_t)oCShaderSystemDrawElements)(_this, pShader, 
		params, 
		pRenderState, 
		vertexCompression, 
		nMaterialVarChangeTimeStamp, 
		nModulationFlags, 
		bRenderingPreTessPatchMesh);

	return;
}

typedef void(__thiscall* CShaderSystemPrepForShaderDrawFunc_t)(void* _this, IShader* pShader, 
	IMaterialVar** ppParams,
	ShaderRenderState_t* pRenderState, int modulation);
void __fastcall hk_CShaderSystem_PrepForShaderDraw(void* _this, void*, IShader* pShader, 
	IMaterialVar** ppParams,
	ShaderRenderState_t* pRenderState, int modulation)
{


	((CShaderSystemPrepForShaderDrawFunc_t)oCShaderSystemPrepForShaderDraw)(_this,
		pShader,
		ppParams,
		pRenderState,
		modulation
		);

	return;
}
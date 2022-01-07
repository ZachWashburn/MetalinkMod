#pragma once
#include "../BaseIncludes.h"

class IShader;
class IMaterialVar;
struct ShaderRenderState_t;

// MOVE THIS!
enum VertexCompressionType_t;

inline LPVOID* oCShaderSystemDrawElements = NULL;
void __fastcall hk_CShaderSystem_DrawElements(void* _this, void*, IShader* pShader, 
	IMaterialVar** params,
	ShaderRenderState_t* pRenderState,
	VertexCompressionType_t vertexCompression,
	uint32 nMaterialVarChangeTimeStamp,
	uint32 nModulationFlags,
	bool bRenderingPreTessPatchMesh);

//E8 ? ? ? ? 0F B6 43 5A
inline LPVOID* oCShaderSystemPrepForShaderDraw = NULL;
void __fastcall hk_CShaderSystem_PrepForShaderDraw(void* _this, void*, IShader* pShader, IMaterialVar** ppParams,
	ShaderRenderState_t* pRenderState, int modulation);


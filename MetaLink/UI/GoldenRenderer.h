#pragma once
#include "../ValveMemoryOverride.h"
#include "../BaseIncludes.h"
// The shit I wrote for Harpoon just ain't gonna cut it for golden gun!
// There is not good info online about this shit. Like I mean none. I tried my best here

//#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)
#define D3DFVF_CUSTOMVERTEX3D (D3DFVF_XYZ | D3DFVF_DIFFUSE)

#define MAX_VERTEX_SIZE 4096*2


inline int g_GRScreenX = 0;
inline int g_GRScreenY = 0;
inline bool g_bNoRender = 0;


struct Coord2D
{
	float x, y = 0.f;
};

struct Coord3D
{
	float x, y, z = 0.f;
};

struct Vertex
{
	D3DXVECTOR4 m_vecPos;
	D3DCOLOR m_Color;
};

struct Vertex3D
{
	D3DXVECTOR3 m_vecPos;
	D3DCOLOR m_Color;
};

struct IndexBuffer
{
	__declspec(noinline) ~IndexBuffer()
	{
#ifdef MEMORY_DEBUGGING
		_ASSERT(_CrtCheckMemory());
#endif
	}

	unsigned long m_nMaxIndexCount = -1;
	int m_nTotalIndexCount = 0;
	int m_nSize = 0;
	unsigned long* m_arrIndices = nullptr;
};


struct PrimitveBatch
{
	__declspec(noinline) ~PrimitveBatch()
	{
#ifdef MEMORY_DEBUGGING
		_ASSERT(_CrtCheckMemory());
#endif
	}
	int m_nCount = 0; // Actual is size!
	int m_nPrimitveCount = 0;
	int m_nVertices = 0;
	D3DPRIMITIVETYPE m_nType = (D3DPRIMITIVETYPE)-1;
	IDirect3DTexture9* m_pTexture; 
	IndexBuffer* m_pIndexBuffer = nullptr;
	D3DCOLOR m_Color = 0;
	bool m_bIsFont = false;
};


class GoldenObject
{
public:
	void Render();
	virtual void DoTransforms() = 0;

private:
	D3DXMATRIX m_matTranslate;
	D3DXVECTOR3 m_vecOrigin;
	D3DXMATRIX m_matRotateX;
	D3DXMATRIX m_matScale;
	Vertex3D* m_pVerts;
	unsigned long* m_nIndicies = 0;
	int m_nVertexCount = 0;
	int m_nIndexCount = 0;
};


class GoldenRenderer;
class RenderList
{
public:
	void Add2DPrimitve(D3DPRIMITIVETYPE nType, Vertex** pVertBuffer,int nVertBufferSize);
	void Add3DPrimitve(D3DPRIMITIVETYPE nType, Vertex3D** pVertBuffer, int nVertBufferSize);
	void AddBatch(PrimitveBatch* pBatch) { m_vecBatch.push_back(pBatch); }
	std::vector<PrimitveBatch*> GetBatches() { return m_vecBatch; };
	std::vector<void*> GetVertices() { return m_arrVerts; };
	void Clear();
	std::uint32_t GetFVF() { return m_nFVF; }
	void SetFVF(std::uint32_t nFVF) { m_nFVF = nFVF; }
	std::uint32_t GetVertexSize() { return m_nVertexSize; }
	void SetVertexSize(std::uint32_t nSize) { m_nVertexSize = nSize; }
	bool Is3D() { return m_bIs3D; }
	void Set3D(bool b3D) { m_bIs3D = b3D; }
private:
	bool m_bIs3D = false;
	std::uint32_t m_nFVF;
	std::uint32_t m_nVertexSize;
	std::vector<void*> m_arrVerts;
	std::vector<PrimitveBatch*> m_vecBatch;
};

class GoldenRenderer {
public:
	GoldenRenderer(IDirect3DDevice9* pDevice);
	void OnBeginScene();
	void OnEndScene();
	void Render();
	void ApplyShader();
	void RenderRenderList(RenderList*);

	// 2D Draw Operations
	void Draw2DTriangle(Coord2D coord1, Coord2D coord2, Coord2D coord3, std::array<int, 4> color);
	void Draw2DTriangle(Coord2D coord1, Coord2D coord2, Coord2D coord3, std::array<int, 4> color, std::array<int, 4> color2, std::array<int, 4> color3);
	void Draw2DRectangle(Coord2D coord1, Coord2D coord2, std::array<int, 4> color);
	void Draw2DRectangle(Coord2D coord1, Coord2D coord2, std::array<int, 4> color, std::array<int, 4> color2);
	void Draw2DRectangle(Coord2D coord1, Coord2D coord2, std::array<int, 4> color, std::array<int, 4> color2, std::array<int, 4> color3, std::array<int, 4> color4);
	void Draw2DRectangleFadeMiddle(Coord2D coord1, Coord2D coord2, std::array<int, 4> color, std::array<int, 4> color2);
	void Draw2DLine(Coord2D coord1, Coord2D coord2, std::array<int, 4> color);
	std::pair<int, int> GetTextSize(int font, const char* szText);
	void _DrawText(int nFont, Coord2D coord1, Coord2D coord2, char* szText, int nTextLength, std::array<int, 4> color);

	// 3D Draw Operations

	void Draw3DTriangle(Coord3D coord1, Coord3D coord2, Coord3D coord3, std::array<int, 4> color);
	void Draw3DLine(Coord3D coord1, Coord3D coord2, std::array<int, 4> color);
	void Clear() { m_pRenderList->Clear(); m_pRenderList3D->Clear(); }
	IDirect3DDevice9* GetDevice() { return m_pDevice; }
	void Render3D();

protected:
	RenderList* m_pRenderList = nullptr;
	RenderList* m_pRenderList3D = nullptr;


	LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer =nullptr;
	IDirect3DIndexBuffer9* m_pIndexBuffer = nullptr;

	LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer3D = nullptr;
	IDirect3DIndexBuffer9* m_pIndexBuffer3D = nullptr;


	IDirect3DDevice9* m_pDevice = nullptr;
	IDirect3DVertexDeclaration9* m_pVertexDecl = nullptr;
	ID3DXEffect* m_pCurrentShader = nullptr;


	D3DXMATRIX m_World;
	D3DXMATRIX m_View;
	D3DXMATRIX m_Projection;
	IDirect3DStateBlock9* m_pinStateBlock;
	IDirect3DStateBlock9* m_pRenderStateBlock;
	std::mutex m_muFrameMutex;

	std::vector<ID3DXFont*> m_vecFonts;
};

inline GoldenRenderer* g_pRenderer;

#pragma once
#include "GoldenRenderer.h"







class RenderData3D
{
public:
	~RenderData3D() { Clear(); }
	void Clear()
	{
		if (m_pVertexData)
			free(m_pVertexData);
		if (m_pIndexData)
			free(m_pIndexData);
		if (m_pClearRect)
			free(m_pClearRect);
	}

	__forceinline void* GetVertexData() { return m_pVertexData; }
	__forceinline int GetVertexSize() { return m_nSizeOfVertexData; }
	__forceinline int GetVertexCount() { return m_nVertexCount; }

	__forceinline unsigned long* GetIndexData() { return m_pIndexData; }
	__forceinline int GetIndexSize() { return m_nSizeOfIndexData; }
	__forceinline int GetIndexCount() { return m_nIndexCount; }

	__forceinline int GetPrimitiveCount() { return m_nPrimitveCount; }
	__forceinline D3DRECT* GetClearRect() { return m_pClearRect; }

//private:
	void* m_pVertexData;
	unsigned long* m_pIndexData;

	int m_nVertexCount = 0;
	int m_nSizeOfVertexData = 0;

	int m_nIndexCount = 0;
	int m_nSizeOfIndexData = 0;

	int m_nPrimitveCount = 0;

	D3DRECT* m_pClearRect;
};




class GoldenRenderer3D
{
public:
	GoldenRenderer3D(LPDIRECT3DDEVICE9 pDevice);
	void OnBeginScene();
	void OnEndScene();
	void Render3DData(RenderData3D* p3DData, D3DXMATRIX matToWorldMatrix, D3DXMATRIX matView, D3DXMATRIX matProjection, bool bClearZBuffer = true, bool bClearScreen = true);
	void SetMaterial(D3DMATERIAL9* pMat) { m_pDevice->SetMaterial(pMat); }
	void SetLighting(D3DLIGHT9* pLight) 
	{ 
		if(pLight)
			m_pDevice->SetLight(0, pLight); 
		m_pDevice->LightEnable(0, (bool)pLight);
	}
	void EnableLighting() { m_pDevice->SetRenderState(D3DRS_LIGHTING, true); }
	void DisableLighting() { m_pDevice->SetRenderState(D3DRS_LIGHTING, false); }
	D3DXMATRIX GetDefaultViewMatrix() {	return m_MatView;}
	D3DXMATRIX GetDefaultProjectionMatrix() { return m_MatProjection; }
private:
	LPDIRECT3DDEVICE9 m_pDevice;    // the pointer to the device class
	LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer = NULL;    // the pointer to the vertex buffer
	IDirect3DIndexBuffer9* m_pIndexBuffer = nullptr;

	D3DXMATRIX m_MatView;
	D3DXMATRIX m_MatProjection;

	IDirect3DStateBlock9* m_pinStateBlock;
	IDirect3DStateBlock9* m_pRenderStateBlock;
};


inline GoldenRenderer3D* g_p3DRenderer = nullptr;


#include "GoldenRenderer.h"


#include "../Resource/Fonts/Ac437_IBM.h"
#include "../Resource/Fonts/Ac437_IBM_BIOS_2y.h"
#include "../Resource/Fonts/Mx437_IBM.h"
GoldenRenderer::GoldenRenderer(IDirect3DDevice9* pDevice)
{
	m_pRenderList = new RenderList;
	m_pRenderList3D = new RenderList;
	m_pRenderList->SetFVF(D3DFVF_CUSTOMVERTEX);
	m_pRenderList->SetVertexSize(sizeof(Vertex));

	m_pRenderList3D->SetFVF(D3DFVF_CUSTOMVERTEX3D);
	m_pRenderList3D->SetVertexSize(sizeof(Vertex3D));
	m_pRenderList3D->Set3D(true);




	m_pDevice = pDevice;
	m_pDevice->AddRef();
	m_pDevice->CreateStateBlock(D3DSBT_ALL, &m_pinStateBlock);

	//m_pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	m_pDevice->CreateStateBlock(D3DSBT_ALL, &m_pRenderStateBlock);


	HRESULT hRet = m_pDevice->CreateVertexBuffer(4000 * sizeof(Vertex),
		0,
		D3DFVF_CUSTOMVERTEX,
		D3DPOOL_DEFAULT,
		&m_pVertexBuffer,
		NULL);

	if (hRet != S_OK || !m_pVertexBuffer) {
		//Debug::QuickPrint(std::to_string(FuckYou).c_str());

		switch (hRet) {
		case D3DERR_INVALIDCALL:
			_asm int 3
			break;
		case D3DERR_OUTOFVIDEOMEMORY:
			_asm int 3
			break;
		case E_OUTOFMEMORY:
			_asm int 3
			break;
		}
		LOGCRITICAL("Unable To Create Vertex Buffer!\n");
		_asm int 3
		_asm int 20
	}

	hRet = m_pDevice->CreateVertexBuffer(4000 * sizeof(Vertex3D),
		0,
		D3DFVF_CUSTOMVERTEX3D,
		D3DPOOL_DEFAULT,
		&m_pVertexBuffer3D,
		NULL);

	if (hRet != S_OK || !m_pVertexBuffer3D) {
		//Debug::QuickPrint(std::to_string(FuckYou).c_str());

		switch (hRet) {
		case D3DERR_INVALIDCALL:
			_asm int 3
			break;
		case D3DERR_OUTOFVIDEOMEMORY:
			_asm int 3
			break;
		case E_OUTOFMEMORY:
			_asm int 3
			break;
		}
		LOGCRITICAL("Unable To Create Vertex Buffer!\n");
		_asm int 3
		_asm int 20
	}


	hRet = m_pDevice->CreateIndexBuffer(4000 * sizeof(DWORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX32, D3DPOOL_DEFAULT,
		&m_pIndexBuffer, NULL);

	if (hRet != S_OK || !m_pIndexBuffer)
	{
		switch (hRet) {
		case D3DERR_INVALIDCALL:
			_asm int 3
			break;
		case D3DERR_OUTOFVIDEOMEMORY:
			_asm int 3
			break;
		case E_OUTOFMEMORY:
			_asm int 3
			break;
		}
		LOG("Unable To Create Index Buffer!\n");
		_asm int 3;
		_asm int 20;
	}


	D3DXMatrixIdentity(&m_World);
	D3DXMatrixIdentity(&m_View);
	D3DXMatrixIdentity(&m_Projection);

	D3DXVECTOR3 eye_vector;
	D3DXVECTOR3 lookat_vector;
	D3DXVECTOR3 up_vector;

	eye_vector = D3DXVECTOR3(0.0f, 0.0f, -8.0f);

	//We are looking towards the origin
	lookat_vector = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	//The "up" direction is the positive direction on the y-axis
	up_vector = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	D3DXMatrixLookAtLH(&m_View, &eye_vector,
		&lookat_vector,
		&up_vector);

	//Since our 'camera' will never move, we can set this once at the
	//beginning and never worry about it again
	m_pDevice->SetTransform(D3DTS_VIEW, &m_View);

	float aspect = ((float)g_GRScreenX / (float)g_GRScreenY);

	D3DXMatrixPerspectiveFovLH(&m_Projection, //Result Matrix
		D3DX_PI / 4,          //Field of View, in radians.
		aspect,             //Aspect ratio
		1.0f,               //Near view plane
		100.0f);           //Far view plane

//Our Projection matrix won't change either, so we set it now and never touch
//it again.
	m_pDevice->SetTransform(D3DTS_PROJECTION, &m_Projection);

	
	void* my_font_data = (void*)(__Mx437_IBM_Model3x_Alt4_ttf); // Your byte array data
	DWORD nFonts = 0, len = sizeof(__Mx437_IBM_Model3x_Alt4_ttf);
	auto H_myfont = AddFontMemResourceEx(my_font_data, len, nullptr, &nFonts);

	if(!H_myfont)
		_asm int 3

	LPD3DXFONT myfont = NULL;     
	D3DXFONT_DESC FontDesc = { 15, 0, 400, 0, false, DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_PITCH, "SuperCustomFont" };     
	D3DXCreateFontIndirect(pDevice, &FontDesc, &myfont);

	if(!myfont)
		_asm int 3

	m_vecFonts.push_back(myfont);

	const char* fontName = XorStr("SuperCustomFont");
	ID3DXFont* pfont;
	auto hr = D3DXCreateFont(m_pDevice,
		20,  // font height
		8,                //Font width

		FW_NORMAL,        //Font Weight

		1,                //MipLevels

		false,            //Italic

		DEFAULT_CHARSET,  //CharSet

		OUT_DEFAULT_PRECIS, //OutputPrecision

		ANTIALIASED_QUALITY, //Quality

		DEFAULT_PITCH | FF_DONTCARE,//PitchAndFamily

		fontName,          //pFacename,

		&pfont);         //ppFont

	m_vecFonts.push_back(pfont);

	fontName = XorStr("SuperCustomFont");
	hr = D3DXCreateFont(m_pDevice,
		11,  // font height
		6,                //Font width

		FW_NORMAL,        //Font Weight

		1,                //MipLevels

		false,            //Italic

		DEFAULT_CHARSET,  //CharSet

		OUT_DEFAULT_PRECIS, //OutputPrecision

		ANTIALIASED_QUALITY, //Quality

		DEFAULT_PITCH | FF_DONTCARE,//PitchAndFamily

		fontName,          //pFacename,

		&pfont);         //ppFont

	m_vecFonts.push_back(pfont);

}

void GoldenRenderer::OnBeginScene()
{
	m_muFrameMutex.lock();
	m_pinStateBlock->Capture();
	m_pRenderStateBlock->Apply();
	m_pDevice->BeginScene();

}


void GoldenRenderer::OnEndScene()
{
	m_pDevice->EndScene();
	m_pinStateBlock->Apply();
	m_muFrameMutex.unlock();
}

void GoldenRenderer::ApplyShader()
{

}


void RenderList::Clear()
{
	for (void* pVertex : m_arrVerts)
	{
#ifdef MEMORY_DEBUGGING
		_ASSERT(_CrtCheckMemory());
#endif
		free(pVertex);
	}

	m_arrVerts.clear();

	for (int i = 0; i < m_vecBatch.size(); i++)
	{
		if (!m_vecBatch.back()->m_bIsFont)
		{
			free(m_vecBatch.back()->m_pIndexBuffer->m_arrIndices);
			delete m_vecBatch.back()->m_pIndexBuffer;
		}
		else
		{
			free(m_vecBatch.back()->m_pTexture);
			free(m_vecBatch.back()->m_pIndexBuffer);
		}
		delete m_vecBatch.back();
#ifdef MEMORY_DEBUGGING
		_ASSERT(_CrtCheckMemory());
#endif
		m_vecBatch.pop_back();
	}
	m_vecBatch.clear();
}

void RenderList::Add2DPrimitve(D3DPRIMITIVETYPE nType, Vertex** pVertBuffer, int nVertBufferSize)
{
	IndexBuffer* pVert;

	if (m_vecBatch.size() && (m_vecBatch.back()->m_nType == nType))
	{
		pVert = m_vecBatch.back()->m_pIndexBuffer;
		m_vecBatch.back()->m_nVertices += nVertBufferSize / sizeof(Vertex);
		m_vecBatch.back()->m_nPrimitveCount++;
		Vertex* pNewMemory = (Vertex*)realloc(m_arrVerts.back(), nVertBufferSize + m_vecBatch.back()->m_nCount);
		_ASSERT(pNewMemory);
		m_arrVerts.back() = pNewMemory;
		memcpy_s((char*)pNewMemory + m_vecBatch.back()->m_nCount, m_vecBatch.back()->m_nCount + nVertBufferSize, *pVertBuffer, nVertBufferSize);
		m_vecBatch.back()->m_nCount += nVertBufferSize;
#ifdef MEMORY_DEBUGGING
		_ASSERT(_CrtCheckMemory());
#endif
	}
	else
	{
		PrimitveBatch* pBatch = new PrimitveBatch;
		m_vecBatch.push_back(pBatch);
		pBatch->m_nType = nType;
		pBatch->m_nCount += nVertBufferSize;
		pBatch->m_pIndexBuffer = new IndexBuffer;
		pVert = pBatch->m_pIndexBuffer;
		pBatch->m_nVertices = nVertBufferSize / sizeof(Vertex);
		pBatch->m_nPrimitveCount++;
		m_arrVerts.push_back(*pVertBuffer);
	}


	if (nType == D3DPT_TRIANGLELIST) // TODO: TriangleList -> TriangleStrip
	{
		// Draw both ways so it doesn't matter our positioning in world space relative to the object
		unsigned long FirstIndex = pVert->m_nMaxIndexCount + 1;
		unsigned long GenericBuffer[] =
		{
			FirstIndex, FirstIndex + 1, FirstIndex + 2,
			FirstIndex, FirstIndex + 2, FirstIndex + 1,
		};

		unsigned int nSizeSave = pVert->m_nSize;
		pVert->m_nSize += sizeof(GenericBuffer);
		unsigned long* pNewMemory = (unsigned long*)realloc(pVert->m_arrIndices, pVert->m_nSize + sizeof(unsigned long));
		if (!pNewMemory)
		{
			LOGCRITICAL("CRITICAL: Failed To Realloc Indices!\n");
			_asm int 3
			_asm int 20
		}
		else
			pVert->m_arrIndices = pNewMemory;


		_ASSERT((pVert->m_arrIndices + nSizeSave) <= (pVert->m_arrIndices + (pVert->m_nSize - sizeof(GenericBuffer))));
		memcpy_s((char*)pVert->m_arrIndices + (int)nSizeSave, pVert->m_nSize, GenericBuffer, sizeof(GenericBuffer));
#ifdef MEMORY_DEBUGGING
		_ASSERT(_CrtCheckMemory());
#endif
		pVert->m_nMaxIndexCount += 3;
		pVert->m_nTotalIndexCount += 6;
		m_vecBatch.back()->m_nPrimitveCount++; //Since we Draw All triangles twice (both ways) we are actually drawing triangles * 2
	}
	else if (nType == D3DPT_LINELIST)
	{
		unsigned long FirstIndex = pVert->m_nMaxIndexCount + 1;
		unsigned long GenericBuffer[] =
		{
			FirstIndex, FirstIndex + 1
		};

		unsigned int nSizeSave = pVert->m_nSize;
		pVert->m_nSize += sizeof(GenericBuffer);
		unsigned long* pNewMemory = (unsigned long*)realloc(pVert->m_arrIndices, pVert->m_nSize);
		if (!pNewMemory)
		{
			LOGCRITICAL("CRITICAL: Failed To Realloc Indices!\n");
			_asm int 3
			_asm int 20
		}
		else
			pVert->m_arrIndices = pNewMemory;


		_ASSERT((pVert->m_arrIndices + nSizeSave) <= (pVert->m_arrIndices + (pVert->m_nSize - sizeof(GenericBuffer))));
		memcpy_s((char*)pVert->m_arrIndices + (int)nSizeSave, pVert->m_nSize, GenericBuffer, sizeof(GenericBuffer));
#ifdef MEMORY_DEBUGGING
		_ASSERT(_CrtCheckMemory());
#endif
		pVert->m_nMaxIndexCount += 2;
		pVert->m_nTotalIndexCount += 2;
	}
}


void RenderList::Add3DPrimitve(D3DPRIMITIVETYPE nType, Vertex3D** pVertBuffer, int nVertBufferSize)
{
	IndexBuffer* pVert;

	if (m_vecBatch.size() && (m_vecBatch.back()->m_nType == nType))
	{
		pVert = m_vecBatch.back()->m_pIndexBuffer;
		m_vecBatch.back()->m_nVertices += nVertBufferSize / sizeof(Vertex3D);
		m_vecBatch.back()->m_nPrimitveCount++;
		Vertex3D* pNewMemory = (Vertex3D*)realloc(m_arrVerts.back(), nVertBufferSize + m_vecBatch.back()->m_nCount);
		_ASSERT(pNewMemory);
		m_arrVerts.back() = pNewMemory;
		memcpy_s((char*)pNewMemory + m_vecBatch.back()->m_nCount, m_vecBatch.back()->m_nCount + nVertBufferSize, *pVertBuffer, nVertBufferSize);
		m_vecBatch.back()->m_nCount += nVertBufferSize;
#ifdef MEMORY_DEBUGGING
		_ASSERT(_CrtCheckMemory());
#endif
	}
	else
	{
		PrimitveBatch* pBatch = new PrimitveBatch;
		m_vecBatch.push_back(pBatch);
		pBatch->m_nType = nType;
		pBatch->m_nCount += nVertBufferSize;
		pBatch->m_pIndexBuffer = new IndexBuffer;
		pVert = pBatch->m_pIndexBuffer;
		pBatch->m_nVertices = nVertBufferSize / sizeof(Vertex3D);
		pBatch->m_nPrimitveCount++;
		m_arrVerts.push_back(*pVertBuffer);
	}


	if (nType == D3DPT_TRIANGLELIST) // TODO: TriangleList -> TriangleStrip
	{
		// Draw both ways so it doesn't matter our positioning in world space relative to the object
		unsigned long FirstIndex = pVert->m_nMaxIndexCount + 1;
		unsigned long GenericBuffer[] =
		{
			FirstIndex, FirstIndex + 1, FirstIndex + 2,
			FirstIndex, FirstIndex + 2, FirstIndex + 1,
		};

		unsigned int nSizeSave = pVert->m_nSize;
		pVert->m_nSize += sizeof(GenericBuffer);
		unsigned long* pNewMemory = (unsigned long*)realloc(pVert->m_arrIndices, pVert->m_nSize + sizeof(unsigned long));
		if (!pNewMemory)
		{
			LOGCRITICAL("CRITICAL: Failed To Realloc Indices!\n");
			_asm int 3
			_asm int 20
		}
		else
			pVert->m_arrIndices = pNewMemory;


		_ASSERT((pVert->m_arrIndices + nSizeSave) <= (pVert->m_arrIndices + (pVert->m_nSize - sizeof(GenericBuffer))));
		memcpy_s((char*)pVert->m_arrIndices + (int)nSizeSave, pVert->m_nSize, GenericBuffer, sizeof(GenericBuffer));
#ifdef MEMORY_DEBUGGING
		_ASSERT(_CrtCheckMemory());
#endif
		pVert->m_nMaxIndexCount += 3;
		pVert->m_nTotalIndexCount += 6;
		m_vecBatch.back()->m_nPrimitveCount++; //Since we Draw All triangles twice (both ways) we are actually drawing triangles * 2
	}
	else if (nType == D3DPT_LINELIST)
	{
		unsigned long FirstIndex = pVert->m_nMaxIndexCount + 1;
		unsigned long GenericBuffer[] =
		{
			FirstIndex, FirstIndex + 1
		};

		unsigned int nSizeSave = pVert->m_nSize;
		pVert->m_nSize += sizeof(GenericBuffer);
		unsigned long* pNewMemory = (unsigned long*)realloc(pVert->m_arrIndices, pVert->m_nSize);
		if (!pNewMemory)
		{
			LOGCRITICAL("CRITICAL: Failed To Realloc Indices!\n");
			_asm int 3
			_asm int 20
		}
		else
			pVert->m_arrIndices = pNewMemory;


		_ASSERT((pVert->m_arrIndices + nSizeSave) <= (pVert->m_arrIndices + (pVert->m_nSize - sizeof(GenericBuffer))));
		memcpy_s((char*)pVert->m_arrIndices + (int)nSizeSave, pVert->m_nSize, GenericBuffer, sizeof(GenericBuffer));
#ifdef MEMORY_DEBUGGING
		_ASSERT(_CrtCheckMemory());
#endif
		pVert->m_nMaxIndexCount += 2;
		pVert->m_nTotalIndexCount += 2;
	}
}

void GoldenRenderer::Draw2DTriangle(Coord2D coord1, Coord2D coord2, Coord2D coord3, std::array<int, 4> color)
{
	Vertex Verts[] =
	{
		Vertex{{coord1.x, coord1.y, 0.0f, 1.0f}, D3DCOLOR_ARGB(color.at(3), color.at(0), color.at(1), color.at(2))},
		Vertex{{coord2.x, coord2.y, 0.0f, 1.0f}, D3DCOLOR_ARGB(color.at(3), color.at(0), color.at(1), color.at(2))},
		Vertex{{coord3.x, coord3.y, 0.0f, 1.0f}, D3DCOLOR_ARGB(color.at(3), color.at(0), color.at(1), color.at(2))},
	};

	int nSize = sizeof(Verts);
	Vertex* pVertBuffer = (Vertex*)malloc(nSize);
	std::memcpy(pVertBuffer, Verts, nSize);
	m_pRenderList->Add2DPrimitve(D3DPT_TRIANGLELIST, &pVertBuffer, nSize);
#ifdef MEMORY_DEBUGGING
	_ASSERT(_CrtCheckMemory());
#endif
}

void GoldenRenderer::Draw2DTriangle(Coord2D coord1, Coord2D coord2, Coord2D coord3, std::array<int, 4> color, std::array<int, 4> color2, std::array<int, 4> color3)
{
	Vertex Verts[] =
	{
		Vertex{{coord1.x, coord1.y, 0.0f, 1.0f}, D3DCOLOR_ARGB(color.at(3), color.at(0), color.at(1), color.at(2))},
		Vertex{{coord2.x, coord2.y, 0.0f, 1.0f}, D3DCOLOR_ARGB(color2.at(3), color2.at(0), color2.at(1), color2.at(2))},
		Vertex{{coord3.x, coord3.y, 0.0f, 1.0f}, D3DCOLOR_ARGB(color3.at(3), color3.at(0), color3.at(1), color3.at(2))},
	};

	int nSize = sizeof(Verts);
	Vertex* pVertBuffer = (Vertex*)malloc(nSize);
	std::memcpy(pVertBuffer, Verts, nSize);
	m_pRenderList->Add2DPrimitve(D3DPT_TRIANGLELIST, &pVertBuffer, nSize);
#ifdef MEMORY_DEBUGGING
	_ASSERT(_CrtCheckMemory());
#endif
}


void GoldenRenderer::Draw3DTriangle(Coord3D coord1, Coord3D coord2, Coord3D coord3, std::array<int, 4> color)
{
	Vertex3D Verts[] =
	{
		Vertex3D{{coord1.x, coord1.y, coord1.z}, D3DCOLOR_ARGB(color.at(3), color.at(0), color.at(1), color.at(2))},
		Vertex3D{{coord2.x, coord2.y, coord2.z}, D3DCOLOR_ARGB(color.at(3), color.at(0), color.at(1), color.at(2))},
		Vertex3D{{coord3.x, coord3.y, coord3.z}, D3DCOLOR_ARGB(color.at(3), color.at(0), color.at(1), color.at(2))},
	};

	int nSize = sizeof(Verts);
	Vertex3D* pVertBuffer = (Vertex3D*)malloc(nSize);
	std::memcpy(pVertBuffer, Verts, nSize);
	m_pRenderList3D->Add3DPrimitve(D3DPT_TRIANGLELIST, &pVertBuffer, nSize);
#ifdef MEMORY_DEBUGGING
	_ASSERT(_CrtCheckMemory());
#endif
}

void GoldenRenderer::Draw3DLine(Coord3D coord1, Coord3D coord2, std::array<int, 4> color)
{
	Vertex3D Verts[] =
	{
		Vertex3D{{coord1.x, coord1.y, coord1.z}, D3DCOLOR_ARGB(color.at(3), color.at(0), color.at(1), color.at(2))},
		Vertex3D{{coord2.x, coord2.y, coord2.z}, D3DCOLOR_ARGB(color.at(3), color.at(0), color.at(1), color.at(2))},
	};

	int nSize = sizeof(Verts);
	Vertex3D* pVertBuffer = (Vertex3D*)malloc(nSize);
	std::memcpy(pVertBuffer, Verts, nSize);
	m_pRenderList3D->Add3DPrimitve(D3DPT_LINELIST, &pVertBuffer, nSize);
#ifdef MEMORY_DEBUGGING
	_ASSERT(_CrtCheckMemory());
#endif
}


void GoldenRenderer::Draw2DRectangle(Coord2D coord1, Coord2D coord2, std::array<int, 4> color)
{
	Draw2DTriangle({ coord1.x, coord1.y }, { coord1.x, coord2.y }, { coord2.x, coord2.y }, color, color, color);
	Draw2DTriangle({ coord2.x, coord2.y }, { coord2.x, coord1.y }, { coord1.x, coord1.y }, color, color, color);
}

void GoldenRenderer::Draw2DRectangle(Coord2D coord1, Coord2D coord2, std::array<int, 4> color, std::array<int, 4> color2, std::array<int, 4> color3, std::array<int, 4> color4)
{
	Draw2DTriangle({ coord1.x, coord1.y }, { coord1.x, coord2.y }, { coord2.x, coord2.y }, color, color3, color2);
	Draw2DTriangle({ coord2.x, coord2.y }, { coord2.x, coord1.y }, { coord1.x, coord1.y }, color2, color4, color);
}

void GoldenRenderer::Draw2DRectangle(Coord2D coord1, Coord2D coord2, std::array<int, 4> color, std::array<int, 4> color2)
{
	Draw2DTriangle({ coord1.x, coord1.y }, { coord1.x, coord2.y }, { coord2.x, coord2.y }, color, color, color2);
	Draw2DTriangle({ coord2.x, coord2.y }, { coord2.x, coord1.y }, { coord1.x, coord1.y }, color2, color, color);
}

void GoldenRenderer::Draw2DRectangleFadeMiddle(Coord2D coord1, Coord2D coord2, std::array<int, 4> color, std::array<int, 4> color2)
{
	Coord2D Difference = { (coord2.x - coord1.x) / 2.f, (coord2.y - coord1.y) / 2.f } ;

	// Top Left Rect
	Draw2DTriangle({ coord1.x, coord1.y }, { coord1.x, coord1.y + Difference.y }, { coord1.x + Difference.x,  coord1.y + Difference.y }, color, color, color2);
	Draw2DTriangle({ coord1.x, coord1.y }, { coord1.x + Difference.x, coord1.y + Difference.y  }, { coord1.x + Difference.x,  coord1.y }, color, color2, color);

	// Top Right Rect
	Draw2DTriangle({ coord1.x + Difference.x, coord1.y  }, { coord2.x, coord1.y  }, { coord1.y + Difference.x,  coord1.y + Difference.y }, color, color, color2);
	Draw2DTriangle({ coord2.x, coord1.y }, { coord2.x , coord1.y + Difference.y }, { coord1.y + Difference.x,  coord1.y + Difference.y }, color, color, color2);

	// Bottom Left Rect
	Draw2DTriangle({ coord1.x, coord1.y + Difference.y }, { coord1.x + Difference.x, coord1.y + Difference.y }, { coord1.x ,  coord2.y }, color, color2, color);
	Draw2DTriangle({ coord1.x + Difference.x, coord1.y + Difference.y }, { coord1.x + Difference.x, coord2.y }, { coord1.x,  coord2.y }, color2, color, color);

	// Bottom Right Rect
	Draw2DTriangle({ coord1.x + Difference.x, coord1.y + Difference.y }, { coord2.x , coord1.y + Difference.y }, { coord2.x,  coord2.y }, color2, color, color);
	Draw2DTriangle({ coord1.x + Difference.x, coord1.y + Difference.y }, { coord2.x,  coord2.y }, {coord1.x + Difference.x, coord2.y}, color2, color, color);

}



void GoldenRenderer::Draw2DLine(Coord2D coord1, Coord2D coord2, std::array<int, 4> color)
{
	Vertex Verts[] =
	{
		Vertex{{coord1.x, coord1.y, 0.0f, 1.0f}, D3DCOLOR_ARGB(color.at(3), color.at(0), color.at(1), color.at(2))},
		Vertex{{coord2.x, coord2.y, 0.0f, 1.0f}, D3DCOLOR_ARGB(color.at(3), color.at(0), color.at(1), color.at(2))},
	};

	int nSize = sizeof(Verts);
	Vertex* pVertBuffer = (Vertex*)malloc(nSize);
	std::memcpy(pVertBuffer, Verts, nSize);
	m_pRenderList->Add2DPrimitve(D3DPT_LINELIST, &pVertBuffer, nSize);
#ifdef MEMORY_DEBUGGING
	_ASSERT(_CrtCheckMemory());
#endif
}



void GoldenRenderer::RenderRenderList(RenderList* pRenderList)
{

	// The 3D Code In Here doesn't actually work, and it's dumb. Do ignore. I didn't exactly understand 3D rendering at the point I attempted this
	// http://www.directxtutorial.com/Lesson.aspx?lessonid=9-4-5
	// 
	if (g_bNoRender || !pRenderList->GetBatches().size())
	{
		pRenderList->Clear();
		return;
	}


	if (m_pCurrentShader)
		ApplyShader();

	int nCurrentVertexPosition = 0;

	IDirect3DVertexBuffer9* pVertBuffer;
	if (pRenderList->Is3D())
		pVertBuffer = m_pVertexBuffer3D;
	else
		pVertBuffer = m_pVertexBuffer;

	std::size_t VertexSize = pRenderList->GetVertexSize();
	m_pDevice->SetFVF(pRenderList->GetFVF());
	for (PrimitveBatch* pBatch : pRenderList->GetBatches())
	{
		if (pBatch->m_bIsFont) // HACK ! HACK ! HACK ! HACK ! HACK ! HACK !
		{
			char* szText = reinterpret_cast<char*>(pBatch->m_pTexture);
			RECT* pRect = reinterpret_cast<RECT*>(pBatch->m_pIndexBuffer);
			m_vecFonts.at(pBatch->m_nCount)->DrawTextA(NULL, szText, -1, pRect, 0, pBatch->m_Color);
			continue;
		}

		void* pStartVertex = pRenderList->GetVertices().at(nCurrentVertexPosition);
		std::vector<void*> Vert = pRenderList->GetVertices();
		void* pVoid;
		void* pVoid2;

		SECTION(__SECTION_VERTEXANDINDEXBUFFER);
		// Copy Over Verts
		pVertBuffer->Lock(0, pBatch->m_nCount, (void**)&pVoid, 0);
		//memcpy_s(pVoid, 4000 * VertexSize, pStartVertex, pBatch->m_nCount);
		memcpy(pVoid, pStartVertex, pBatch->m_nCount);
		pVertBuffer->Unlock();

		// Copy Over Indexes
		m_pIndexBuffer->Lock(0, pBatch->m_pIndexBuffer->m_nSize, (void**)&pVoid2, 0);
		//memcpy_s(pVoid2, 4000 * sizeof(DWORD), pBatch->m_pIndexBuffer->m_arrIndices, pBatch->m_pIndexBuffer->m_nSize);
		memcpy(pVoid2, pBatch->m_pIndexBuffer->m_arrIndices, pBatch->m_pIndexBuffer->m_nSize);
		m_pIndexBuffer->Unlock();
		ENDSECTION(__SECTION_VERTEXANDINDEXBUFFER);

		// Set And Draw 
		SECTION(__SECTION_DRAWINDEXEDPRIMITIVE);
		m_pDevice->SetStreamSource(0, pVertBuffer, 0, pRenderList->GetVertexSize());
		m_pDevice->SetIndices(m_pIndexBuffer);
		HRESULT hRes = m_pDevice->DrawIndexedPrimitive(pBatch->m_nType, 0, 0, pBatch->m_nVertices, 0, pBatch->m_nPrimitveCount);
		ENDSECTION(__SECTION_DRAWINDEXEDPRIMITIVE);
		_ASSERT(hRes == D3D_OK)
#if 0
			for (int i = 0; i < pBatch->m_pIndexBuffer->m_nTotalIndexCount; i++)
			{
				VCON("%d", pBatch->m_pIndexBuffer->m_arrIndices[i]);
			}
		VCON("\n");
#endif
		nCurrentVertexPosition++;
	}


	//pRenderList->Clear();
#ifdef MEMORY_DEBUGGING
	_ASSERT(_CrtCheckMemory());
#endif

}

std::pair<int, int> GoldenRenderer::GetTextSize(int font, const char* szText) {
	RECT rcRect = { 0,0,0,0 };
	if (m_vecFonts.at(font))
	{
		m_vecFonts.at(font)->DrawTextA(NULL, szText, strlen(szText), &rcRect, DT_CALCRECT,
			D3DCOLOR_XRGB(0, 0, 0));
	}
	return std::pair<int, int>(rcRect.right - rcRect.left,  rcRect.bottom - rcRect.top );
}


void GoldenRenderer::_DrawText(int font, Coord2D coord1, Coord2D coord2, char* szText, int nTextLength, std::array<int, 4> color)
{ // HACK ! HACK ! HACK ! HACK ! HACK ! HACK !
	if (!nTextLength)
		nTextLength = strlen(szText);


	PrimitveBatch* pBatch = new PrimitveBatch;
	pBatch->m_nCount = font;
	pBatch->m_bIsFont = true;
	pBatch->m_pTexture = (IDirect3DTexture9*)malloc((nTextLength + 1) * sizeof(char));
	pBatch->m_pIndexBuffer = (IndexBuffer*)malloc(sizeof(RECT));
	RECT* pRect = (RECT*)pBatch->m_pIndexBuffer;
	pRect->left = coord1.x;
	pRect->right = coord2.x;
	pRect->top = coord1.y;
	pRect->bottom = coord2.y;
	strcpy_s((char*)pBatch->m_pTexture, (nTextLength + 1) * sizeof(char), szText);
	pBatch->m_Color = D3DCOLOR_ARGB(color.at(3), color.at(0), color.at(1), color.at(2));
	m_pRenderList->AddBatch(pBatch);
}

void GoldenRenderer::Render()
{
	SCOPESECTION(__SECTION_RENDER)

	m_pDevice->SetTexture(0, 0);
	m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pDevice->SetPixelShader(0);
	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	RenderRenderList(m_pRenderList); // Render 2D 




#ifdef MEMORY_DEBUGGING
		_ASSERT(_CrtCheckMemory());
#endif

}





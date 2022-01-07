#include "3DGoldenRenderer.h"



GoldenRenderer3D::GoldenRenderer3D(LPDIRECT3DDEVICE9 pDevice)
{
	m_pDevice = pDevice;
	m_pDevice->AddRef();

	m_pDevice->CreateStateBlock(D3DSBT_ALL, &m_pinStateBlock);

	m_pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);    // turn off the 3D lighting for now



	m_pDevice->CreateStateBlock(D3DSBT_ALL, &m_pRenderStateBlock);
    m_pRenderStateBlock->Capture();

    // create a vertex buffer interface called v_buffer
    m_pDevice->CreateVertexBuffer(10000 * sizeof(Vertex3D),    // change to 4, instead of 3
        0,
        D3DFVF_CUSTOMVERTEX3D,
        D3DPOOL_DEFAULT,
        &m_pVertexBuffer,
        NULL);

    m_pDevice->CreateIndexBuffer(10000 * sizeof(DWORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX32, D3DPOOL_DEFAULT,
        &m_pIndexBuffer, NULL);



    D3DXVECTOR3 vec1 = D3DXVECTOR3(0.0f, 0.0f, 100.0f);
    D3DXVECTOR3 vec2 = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 vec3 = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
    D3DXMatrixLookAtLH(&m_MatView,
        &vec1,    // the camera position
        &vec2,    // the look-at position
        &vec3);    // the up direction

    D3DXMatrixPerspectiveFovLH(&m_MatProjection,
        D3DXToRadian(70),    // the horizontal field of view
        (FLOAT)g_GRScreenX / (FLOAT)g_GRScreenY, // aspect ratio
        1.0f,    // the near view-plane
        500.0f);    // the far view-plane

}

void GoldenRenderer3D::OnBeginScene()
{

    m_pinStateBlock->Capture();
    m_pRenderStateBlock->Apply();
    m_pDevice->BeginScene();    // begins the 3D scene

    m_pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
    m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);    // turn off the 3D lighting for now

}

void GoldenRenderer3D::OnEndScene()
{
    m_pDevice->EndScene();    // begins the 3D scene
    m_pinStateBlock->Apply();
}

void GoldenRenderer3D::Render3DData(RenderData3D* p3DData, D3DXMATRIX matToWorldMatrix, D3DXMATRIX matView, D3DXMATRIX matProjection, bool bClearZBuffer /*= true*/, bool bClearScreen /*= true*/)
{
    SCOPESECTION(__SECTION_RENDER3D);

    m_pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);


    if (bClearScreen)
        m_pDevice->Clear(0, p3DData->GetClearRect(), D3DCLEAR_TARGET, D3DCOLOR_XRGB(150, 150, 150), 1.0f, 0);
    if (bClearZBuffer)
        m_pDevice->Clear(0, p3DData->GetClearRect(), D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

    // Copy Our Data Over

    VOID* pVoid;    // a void pointer
    VOID* pVoid2;    // a void pointer
    // lock v_buffer and load the vertices into it
    int nMaxDataSize = (10000 * sizeof(unsigned long));
    int nTimesToSplit = (p3DData->GetIndexSize() / nMaxDataSize);
    int nDataLeft = p3DData->GetIndexSize();
    int nTotalData = nDataLeft;


    int nVertDataCopied = 0;
    int nIndiceDataCopied = 0;

    for (int i = 0; nIndiceDataCopied < nTotalData; i++)
    {
        int nDataToCopy = min(nDataLeft, nMaxDataSize);
        int nIndexesToCopy = nDataToCopy / sizeof(unsigned long);
        nDataToCopy -= (nIndexesToCopy % 3) * sizeof(unsigned long);

        // Making the dangerous assumption we double every index!!!!
        int nVerts = (nDataToCopy / sizeof(unsigned long));// / 2;

        if (!nVerts)
            break;

        m_pVertexBuffer->Lock(0, 0, (void**)&pVoid, 0);
        memset(pVoid, 0, 10000 * sizeof(Vertex3D));
        memcpy(pVoid, (char*)p3DData->GetVertexData() + nVertDataCopied, nVerts * sizeof(Vertex3D));
        m_pVertexBuffer->Unlock();

        nVertDataCopied += nVerts * sizeof(Vertex3D);

        m_pIndexBuffer->Lock(0, 0, (void**)&pVoid2, 0);
        memset(pVoid2, 0, 10000 * sizeof(DWORD));
        memcpy_s(pVoid2, 10000 * sizeof(DWORD), (char*)p3DData->GetIndexData() + nIndiceDataCopied, nDataToCopy);
        m_pIndexBuffer->Unlock();

        nIndiceDataCopied += nDataToCopy;

        m_pDevice->SetFVF(D3DFVF_CUSTOMVERTEX3D);

        // Do Transforms
        m_pDevice->SetTransform(D3DTS_WORLD, &matToWorldMatrix);
        m_pDevice->SetTransform(D3DTS_VIEW, &matView);    // set the view transform to matView
        m_pDevice->SetTransform(D3DTS_PROJECTION, &matProjection);    // set the projection

        // select the vertex buffer to display
        m_pDevice->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(Vertex3D));
        m_pDevice->SetIndices(m_pIndexBuffer);
        HRESULT hRes = m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, p3DData->GetVertexCount(), 0, p3DData->GetPrimitiveCount());

    }
}
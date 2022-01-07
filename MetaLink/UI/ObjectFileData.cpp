#include "ObjectFileData.h"
#include "../ThirdParty/OBJ_Loader.h"

ObjectFile3DData::ObjectFile3DData()
{
	m_pLoader = new objl::Loader();
}

ObjectFile3DData::ObjectFile3DData(const char* szFilePath)
{
	m_pLoader = new objl::Loader();
}


void ObjectFile3DData::Render()
{
	D3DXMATRIX matRotateY;    // a matrix to store the rotation information
	D3DXMATRIX matRotateZ;
	D3DXMATRIX matRotateX;
	D3DXMATRIX matTranslationZ;

	static float index = 0.0f; index += 0.01f;    // an ever-increasing float value

	float s = 0.2f;
	D3DXMATRIX scale;
	// build a matrix to rotate the model based on the increasing float value
	
	D3DXMatrixRotationZ(&matRotateZ, 0);
	D3DXMatrixRotationX(&matRotateX, 0);
	D3DXMatrixScaling(&scale, s, s, s);
	D3DXMatrixRotationY(&matRotateY, index);
	D3DXMatrixTranslation(&matTranslationZ, 0, -40, -10);

	D3DXMATRIX out;
	D3DXMatrixMultiply(&out, &scale, &matRotateY);
	D3DXMatrixMultiply(&out, &out, &matRotateZ);
	D3DXMatrixMultiply(&out, &out, &matRotateX);
	D3DXMatrixMultiply(&out, &out, &matTranslationZ);
	

	D3DLIGHT9 light;
	D3DMATERIAL9 m_Mat;

	ZeroMemory(&light, sizeof(light));
	// clear out the light struct for use
	light.Type = D3DLIGHT_POINT;
	// make the light type 'directional light'
	light.Diffuse.r = 1.0f;
	light.Diffuse.g = 1.0f;
	light.Diffuse.b = 1.0f;
	light.Diffuse.a = 1.0f;
	light.Ambient.r = 1.0f;
	light.Ambient.g = 1.0f;
	light.Ambient.b = 1.0f;
	light.Specular.r = 1.0f;
	light.Specular.g = 1.0f;
	light.Specular.b = 1.0f;
	// set the lighting position
	light.Position.x = 0;
	light.Position.y = -10;
	light.Position.z = -30;
	light.Range = 9000.0f;
	light.Attenuation0 = 0.0f;
	light.Attenuation1 = 0.040f;
	light.Attenuation2 = 0.0f;
	g_p3DRenderer->EnableLighting();
	g_p3DRenderer->SetLighting(&light);

	ZeroMemory(&m_Mat, sizeof(m_Mat));
	// clear out the struct for use
	m_Mat.Diffuse.r = 1.0f;
	m_Mat.Diffuse.g = 1.0f;
	m_Mat.Diffuse.b = 0.60f;
	m_Mat.Diffuse.a = 1.0f;
	// set diffuse color to white
	m_Mat.Ambient.r = 1.0f;
	m_Mat.Ambient.g = 1.0f;
	m_Mat.Ambient.b = 0.60f;
	m_Mat.Ambient.a = 1.0f;
	// set ambient color to white
	m_Mat.Specular.r = 1.0f;
	m_Mat.Specular.g = 1.0f;
	m_Mat.Specular.b = 0.60f;
	m_Mat.Specular.a = 1.0f;
	m_Mat.Power = 500.0f;

	g_p3DRenderer->SetMaterial(&m_Mat);

	g_p3DRenderer->Render3DData(this, out, g_p3DRenderer->GetDefaultViewMatrix(), g_p3DRenderer->GetDefaultProjectionMatrix(), true, false);
	g_p3DRenderer->DisableLighting();
	g_p3DRenderer->SetLighting(NULL);
}

bool ObjectFile3DData::ParseFromFile(const char* szFilePath)
{
	if (!m_pLoader->LoadFile(szFilePath))
	{
		VCON(XorStr("Unable To Load Object File %s\n"), szFilePath);
		if(IsDebuggerPresent())
			_asm int 3
		return false;
	}
#if 1
	int i_vert = 0;
	int i_index = 0;
	for (objl::Mesh& curr_mesh : m_pLoader->LoadedMeshes)
	{

		m_nVertexCount += curr_mesh.Vertices.size();
		m_nSizeOfVertexData += m_nVertexCount * sizeof(Vertex3D);
		m_pVertexData = realloc(m_pVertexData, m_nSizeOfVertexData);

		m_nPrimitveCount = m_nVertexCount / 3;

		for (objl::Vertex& vert : curr_mesh.Vertices)
		{
			Vertex3D vert3d = { {vert.Position.X, vert.Position.Y, vert.Position.Z}, D3DCOLOR_ARGB(255, 255, 215, 0) };
			memcpy((char*)m_pVertexData + (i_vert * sizeof(Vertex3D)), &vert3d, sizeof(vert3d));
			i_vert++;
		}

		m_nIndexCount += curr_mesh.Indices.size();// *2;
		m_nSizeOfIndexData += m_nIndexCount * sizeof(unsigned long);
		m_pIndexData = (unsigned long*)realloc(m_pIndexData, m_nSizeOfIndexData);

		
		for (int i = 0; i < (curr_mesh.Indices.size()); i++)
		{
#if 0
			unsigned long Indexes[] =
			{
				curr_mesh.Indices.at(i), curr_mesh.Indices.at(i + 1), curr_mesh.Indices.at(i + 2),
				//curr_mesh.Indices.at(i), curr_mesh.Indices.at(i + 2), curr_mesh.Indices.at(i + 1)
			};
			memcpy((char*)m_pIndexData + (i_index * sizeof(Indexes)), &Indexes, sizeof(Indexes));		
#endif
			m_pIndexData[i_index] = curr_mesh.Indices.at(i);
			i_index++;
		}
	}
#endif
	for (int i = 0; i < m_nIndexCount; i+=3)
	{
		VCON("Index : %d , %d, %d\n", m_pIndexData[i], m_pIndexData[i + 1], m_pIndexData[i + 2]);
	}
	m_nPrimitveCount = (m_nIndexCount / 3);// *2;

}

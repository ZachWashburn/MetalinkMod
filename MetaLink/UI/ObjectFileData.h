#pragma once
#include "3DGoldenRenderer.h"


namespace objl
{
	class Loader;
}


class ObjectFile3DData : public RenderData3D
{
public:
	ObjectFile3DData();
	ObjectFile3DData(const char* szFilePath);
	virtual void Render();
	virtual bool __declspec(noinline) ParseFromFile(const char* szFileName);

private:
	objl::Loader* m_pLoader;
};
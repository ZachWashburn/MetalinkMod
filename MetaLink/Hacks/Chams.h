#pragma once
#include "../BaseIncludes.h"
#include "../Configuration.h"
#include <map>
#include "../ThirdParty/fnv.h"
class Entity;
class IMaterial;


class IMaterial;

class ChamRenderer
{
public:
	ChamRenderer();
	void DrawSceneEndCham(Config::Cham_s::Material Mat, Entity* pEnt);
	void OnSceneEnd();
	void __forceinline SceneEndPlayerCham(Entity*);
	void __forceinline DrawPlayerSceneEndCham(Entity* pEnt, Config::Cham_s::Material Materials[10]);
private:
	IMaterial* DispatchMaterial(uint32_t nMaterial)
	{
		return m_mapMaterials[nMaterial];
	}
	__forceinline IMaterial* DispatchMaterial(std::string strMaterial)
	{
		return DispatchMaterial(fnv::hashRuntime(strMaterial.c_str()));
	}
	std::map<uint32_t, IMaterial*> m_mapMaterials;
};


inline ChamRenderer* g_pChamRenderer = nullptr;
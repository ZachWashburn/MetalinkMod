#include "Chams.h"
#include "SDK.h"
#include "IMaterial.h"
#include "../Configuration.h"

ChamRenderer::ChamRenderer()
{
#if 1
    {
        //selfillumfresnelminmaxexp 0 0 0 0
        const auto kv = KeyValues::FromString(XorStr("VertexLitGeneric"), XorStr(""));
        kv->SetString(XorStr("$envmaptint"), XorStr("[1 1 1]"));
        kv->SetString(XorStr("$envmapcontrast"), XorStr("0"));

        kv->SetString(XorStr("$selfillum"), XorStr("1"));
        kv->SetString(XorStr("$selfillumfresnel"), XorStr("1"));
        kv->SetString(XorStr("$selfillummaskscale"), XorStr("1"));
        m_mapMaterials[fnv::hash("Normal")] = g_pInterfaces->m_pMaterialSystem->CreateMaterial(XorStr("normalmcsi"), kv);
    }



#endif




}



void ChamRenderer::DrawSceneEndCham(Config::Cham_s::Material Mat, Entity* pEnt) {
#if 1
    IMaterial* pMaterial = DispatchMaterial(Mat.MaterialName);
    if (!pMaterial)
        return;

    if (Mat.bCallOriginalInSceneEnd) {
        pEnt->DrawModel(0x1, RenderableInstance_t( 255 ));
        return;
    }

    float r, g, b, a;
    r = Mat.color->at(0);
    g = Mat.color->at(1);
    b = Mat.color->at(2);
    a = Mat.color->at(3);

    pMaterial->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, Mat.Opts.bIgnorez);
    pMaterial->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, Mat.Opts.bWireframe);

    //if (Mat.MaterialName == "glow" || Mat.MaterialName == "chrome" || Mat.MaterialName == "plastic" || Mat.MaterialName == "glass" || Mat.MaterialName == "crystal")
   // //    pMaterial->FindVar("$envmaptint")->setVectorValue(r, g, b);
    //else {
       pMaterial->ColorModulate(r, g, b);
   // }

   // if (Mat.MaterialName == "glow")
        //pMaterial->FindVar("$envmapfresnelminmaxexp")->setVecComponentValue(9.0f * (1.2f - pulse), 2);
    //else
       pMaterial->AlphaModulate(a);




    g_pInterfaces->m_pRenderView->SetColorModulation(r, g, b);
    g_pInterfaces->m_pRenderView->SetBlend(a);
    g_pInterfaces->m_pStudioRender->ForcedMaterialOverride(pMaterial);
    RenderableInstance_t Instance;
    Instance.m_nAlpha = Mat.color.a();
    pEnt->DrawModel(0x1, Instance);
    g_pInterfaces->m_pRenderView->SetBlend(1.f);
    g_pInterfaces->m_pStudioRender->ForcedMaterialOverride(nullptr);
#endif
}


void ChamRenderer::DrawPlayerSceneEndCham(Entity* pEnt, Config::Cham_s::Material Materials[10])
{
    for (int i = 0; i < 10; i++) {

        Config::Cham_s::Material Mat = Materials[i];

        if (!Mat.bRenderInSceneEnd || !Mat.bEnabled || !Mat.Opts.bIgnorez)
            continue;

        DrawSceneEndCham(Mat, pEnt);
    }
    Config::Cham_s::Material NMat;
    NMat.bCallOriginalInSceneEnd = true;
    DrawSceneEndCham(NMat, pEnt);

    for (int i = 0; i < 10; i++) {

        Config::Cham_s::Material Mat = Materials[i];

        if (!Mat.bRenderInSceneEnd || !Mat.bEnabled || Mat.Opts.bIgnorez)
            continue;

        DrawSceneEndCham(Mat, pEnt);
    }


}

void __forceinline ChamRenderer::SceneEndPlayerCham(Entity* pEnt)
{
    if ((pEnt->m_iTeamNum() != g_pLocalPlayer->m_iTeamNum()) && g_pConfig->m_mapChams[fnv::hash("Enemies")].bEnabled)
    {
        DrawPlayerSceneEndCham(pEnt, g_pConfig->m_mapChams[fnv::hash("Enemies")].Materials);
    }
}


void ChamRenderer::OnSceneEnd()
{
	for(int i = 0; i < g_pInterfaces->m_pEngine->GetMaxClients() ; i++)
	{
		Entity* pEnt = g_pInterfaces->m_pEntityList->GetClientEntity(i);
		
		if (!pEnt)
			continue;

        SceneEndPlayerCham(pEnt);
	}
}
#include "ESP.h"
#include "../BaseIncludes.h"
#include "../UI/GoldenRenderer.h"
#include "../Configuration.h"
#include "SDK.h"

struct Point {
	int x;
	int y;
};

struct Rect {
	float x;
	float y;
	float w;
	float h;
};

float dotProduct(const Vector& v, Vector Vec2) 
{
	return Vec2.x * v.x + Vec2.y * v.y + Vec2.z * v.z;
}

Vector TransformVecAndMatrix(Vector vec, const matrix3x4& mat) 
{
	return  Vector{ DotProduct(vec, { mat[0][0], mat[0][1], mat[0][2] }) + mat[0][3],
		DotProduct(vec, { mat[1][0], mat[1][1], mat[1][2] }) + mat[1][3],
		 DotProduct(vec, { mat[2][0], mat[2][1], mat[2][2] }) + mat[2][3]
	};			 				  
}



bool GetBBoxMinMax2D(Vector& mins, Vector& maxs, Vector2D& maxO, Vector2D& minO,const matrix3x4* matrix = nullptr) {
	std::array<float, 3> scale = { 1.f,1.f,1.f };
	minO.y = minO.x = (std::numeric_limits<float>::max)();
	maxO.y = maxO.x = -((std::numeric_limits<float>::max)());

	//const auto scaledMins = mins + (maxs - mins) * 2 * (0.25f - 1.f);
	//const auto scaledMaxs = maxs - (maxs - mins) * 2 * (0.25f - 1.f);
	const auto scaledMins = mins;
	const auto scaledMaxs = maxs;

	std::array<Coord2D, 8> vertices;

	for (int i = 0; i < 8; ++i) {
		const Vector point{ i & 1 ? scaledMaxs.x : scaledMins.x,
							i & 2 ? scaledMaxs.y : scaledMins.y,
							i & 4 ? scaledMaxs.z : scaledMins.z };

		if (!WorldToScreen(matrix ? TransformVecAndMatrix(point, *matrix) : point, vertices[i].x, vertices[i].y)) {
			return false;
		}

		minO.x = (std::min)(minO.x, vertices[i].x);
		minO.y = (std::min)(minO.y, vertices[i].y);
		maxO.x = (std::max)(maxO.x, vertices[i].x);
		maxO.y = (std::max)(maxO.y, vertices[i].y);
	}
	return true;
}



bool GetPlayerBoxRect(Entity* player, Rect& box) { // This is harder than any exploit i've ever found
	Vector origin, min, max;
	origin = player->GetAbsOrigin();

	Vector2D Min2D, Max2D, Org2D;
	//matrix3x4_t bones[256];
	//player->SetupBones(bones, 256, 256, gpGlobals->m_flCurtime);

	const matrix3x4_t& mat = player->RenderableToWorldTransform();

	ICollideable* pCollide = player->GetCollideable();
	min = pCollide->OBBMins();
	max = pCollide->OBBMaxs();
	if (!GetBBoxMinMax2D(min, max, Min2D, Max2D, &mat)) {
		return false;
	}

	if (!WorldToScreen(origin, Org2D))
		return false;

	//float diff = Max2D.y - Min2D.y;
	//Min2D.y = Org2D.y;
	//Max2D.y = Min2D.y += diff;

	box.x = Min2D.x;
	box.y = Org2D.y - 6;
	box.w = Max2D.x - Min2D.x;
	box.h = (Max2D.y - Min2D.y) + 6;


	return true;
}

bool GetPlayerBoxVectors(Entity* player, Vector& mins, Vector& maxs) {
	Vector origin;
	Vector2D bottom, top;
	origin = player->GetAbsOrigin();
	player->ComputeHitboxSurroundingBox(&mins, &maxs, 0, 0);
	mins = { origin.x, origin.y, mins.z };
	maxs = { origin.x, origin.y, maxs.z + 8.f };
	return true;
}

extern QAngle CalcAngle(Vector& src, Vector& dst);


void RenderPlayerBox(Entity* pEnt, ColorToggle* pToggle)
{
	if (!pToggle->bEnabled)
		return;

	Rect recBox;
	if (!GetPlayerBoxRect(pEnt, recBox))
		return;

	std::array<int, 4> color = { int(pToggle->color->at(0) * 255),int(pToggle->color->at(1) * 255), int(pToggle->color->at(2) * 255), int(pToggle->color->at(3) * 255) };
	g_pRenderer->Draw2DLine({ recBox.x, recBox.y }, { recBox.x + recBox.w, recBox.y }, color);
	g_pRenderer->Draw2DLine({ recBox.x + recBox.w, recBox.y }, { recBox.x + recBox.w, recBox.y + recBox.h }, color);
	g_pRenderer->Draw2DLine({ recBox.x, recBox.y + recBox.h }, { recBox.x + recBox.w, recBox.y + recBox.h }, color);
	g_pRenderer->Draw2DLine({ recBox.x, recBox.y + recBox.h }, { recBox.x, recBox.y }, color);

	recBox.x -= 1;
	recBox.y -= 1;
	recBox.w += 2;
	recBox.h += 2;

	g_pRenderer->Draw2DLine({ recBox.x, recBox.y }, { recBox.x + recBox.w, recBox.y }, color);
	g_pRenderer->Draw2DLine({ recBox.x + recBox.w, recBox.y }, { recBox.x + recBox.w, recBox.y + recBox.h }, color);
	g_pRenderer->Draw2DLine({ recBox.x, recBox.y + recBox.h }, { recBox.x + recBox.w, recBox.y + recBox.h }, color);
	g_pRenderer->Draw2DLine({ recBox.x, recBox.y + recBox.h }, { recBox.x, recBox.y }, color);

	recBox.x -= 1;
	recBox.y -= 1;
	recBox.w += 2;
	recBox.h += 2;

	g_pRenderer->Draw2DLine({ recBox.x, recBox.y }, { recBox.x + recBox.w, recBox.y }, { 0,0,0,255 });
	g_pRenderer->Draw2DLine({ recBox.x + recBox.w, recBox.y }, { recBox.x + recBox.w, recBox.y + recBox.h }, { 0,0,0,255 });
	g_pRenderer->Draw2DLine({ recBox.x, recBox.y + recBox.h }, { recBox.x + recBox.w, recBox.y + recBox.h }, { 0,0,0,255 });
	g_pRenderer->Draw2DLine({ recBox.x, recBox.y + recBox.h }, { recBox.x, recBox.y }, { 0,0,0,255 });

	recBox.x += 3;
	recBox.y += 3;
	recBox.w -= 6;
	recBox.h -= 6;

	g_pRenderer->Draw2DLine({ recBox.x, recBox.y }, { recBox.x + recBox.w, recBox.y }, { 0,0,0,255 });
	g_pRenderer->Draw2DLine({ recBox.x + recBox.w, recBox.y }, { recBox.x + recBox.w, recBox.y + recBox.h }, { 0,0,0,255 });
	g_pRenderer->Draw2DLine({ recBox.x, recBox.y + recBox.h }, { recBox.x + recBox.w, recBox.y + recBox.h }, { 0,0,0,255 });
	g_pRenderer->Draw2DLine({ recBox.x, recBox.y + recBox.h }, { recBox.x, recBox.y }, { 0,0,0,255 });
}


void RenderPlayerESPText(Entity* pEnt, ColorToggle* pToggle)
{


	if (!pToggle->bEnabled)
		return;

	Rect recBox;
	if (!GetPlayerBoxRect(pEnt, recBox))
		return;

	int font = 1;
	if (g_pLocalPlayer->GetEyePosition().DistTo(pEnt->GetAbsOrigin()) > 400.f)
		font = 0;
	if (g_pLocalPlayer->GetEyePosition().DistTo(pEnt->GetAbsOrigin()) > 1000.f)
		font = 2;

	float xpos = recBox.x + recBox.w + 2.f;
	float ypos = recBox.y;
	player_info_t pInfo;
	g_pInterfaces->m_pEngine->GetPlayerInfo(pEnt->Index(), &pInfo);
	auto size = g_pRenderer->GetTextSize(font, pInfo.name);
	g_pRenderer->_DrawText(font, { xpos + 2.f,ypos + 2.f }, { float{xpos + size.first + 2.f }, float{ ypos + size.second + 2.f } }, pInfo.name, strlen(pInfo.name), { 0,0,0,255 });
	g_pRenderer->_DrawText(font, { xpos + 1.f,ypos + 1.f }, { float{xpos + size.first + 1.f }, float{ ypos + size.second + 1.f} }, pInfo.name, strlen(pInfo.name), { 100,100,100,255 });
	g_pRenderer->_DrawText(font, { xpos,ypos }, { float{xpos + size.first }, float{ ypos + size.second } }, pInfo.name, strlen(pInfo.name), { 255,255,255,255 });

}


void ESP::OnPresent()
{
	if (!g_pConfig->m_ESP.m_bEnabled || !g_pLocalPlayer.Get() || !g_pInterfaces->m_pEngine->IsInGame() || !g_pInterfaces->m_pEngine->IsConnected())
		return;



	for (int i = 0; i < g_pInterfaces->m_pEngine->GetMaxClients(); i++)
	{
		Entity* pEnt = g_pInterfaces->m_pEntityList->GetClientEntity(i);
		if (!pEnt || (pEnt == g_pLocalPlayer.Get()) || !pEnt->IsPlayer() || pEnt->IsDormant() || !pEnt->IsAlive())
			continue;

		if (pEnt->m_iTeamNum() != g_pLocalPlayer->m_iTeamNum())
		{
			RenderPlayerBox(pEnt, &(g_pConfig->m_ESP.m_EnemyESP.ctBox));
			RenderPlayerESPText(pEnt, &(g_pConfig->m_ESP.m_EnemyESP.ctName));
		}
		if (pEnt->m_iTeamNum() == g_pLocalPlayer->m_iTeamNum())
		{
			RenderPlayerBox(pEnt, &(g_pConfig->m_ESP.m_TeammateESP.ctBox));
			RenderPlayerESPText(pEnt, &(g_pConfig->m_ESP.m_TeammateESP.ctName));
		}
	}



	for (int i = 0; i < g_pInterfaces->m_pEntityList->GetMaxEntities(); i++)
	{
		Entity* pEnt = g_pInterfaces->m_pEntityList->GetClientEntity(i);

		if (!pEnt || !pEnt->IsAlive() || pEnt->IsPlayer() || !pEnt->IsWeapon() || (pEnt->m_iItemDefinitionIndex() != 19))
			continue;

		RenderPlayerBox(pEnt, &(g_pConfig->m_ESP.m_TeammateESP.ctBox));
		//pEnt->DrawDebugOverlay();

		//player_info_t player_info;
		//g_pInterfaces->m_pEngine->GetPlayerInfo(i, &player_info);	
		// static int offset = FF  90 ? ? ? ? 00  00  85  C0  0F  8F ? ? ? ? ? ? ? ? 80  BE ? ? ? ? 00  00  02 + 16
		//static int TakeDamageOffset = *(int*)((char*)MemoryTools::FindPattern(__ENGINE, "\xFF\x90????\x00\x00\x85\xC0\x0F\x8F????????\x80\xBE????\x00\x00\x02", 0, 0) + 16);
		//int nTakeDamage = *(int*)(pEnt + TakeDamageOffset);
		//VCON("Player %s (%d) Has Collison Group %d, Solid is %d, SolidFlags %d\n", player_info.name, i, pEnt->m_iCollisionGroup(), pEnt->m_nSolidType(), pEnt->m_usSolidFlags());
	}

}
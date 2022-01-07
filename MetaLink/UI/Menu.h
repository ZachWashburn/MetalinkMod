#pragma once
#include "../BaseIncludes.h"
#include "Pendelum3.h"



class Menu
{
public:
	void OnPresent();
	void Render();
	bool IsMenuOpen() { return m_bIsMenuOpen; }
	void Init();
	bool OnLockCursor();
	void Define();
	bool HasAnimationFinished();
	void SetSize(std::pair<float, float> size) { m_flpLastWindowSize = size; }
	bool IsOpen() { return m_bIsMenuOpen; }
private:
	bool m_bIsMenuOpen{ false };
	Pendelum3::Pendelum3RenderInstance* m_pRenderInstance;
	std::pair<float, float> m_flpLastWindowSize;
};

inline Menu g_Menu;
inline Menu* g_pMenu = &g_Menu;
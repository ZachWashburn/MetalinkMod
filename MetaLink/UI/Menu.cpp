#include "Menu.h"
#include "Pendelum3.h"
#include "SDK.h"
#include "../Configuration.h"


enum MenuTabs
{
	__TAB_ESP,
	__TAB_CHAMS,
	__TAB_AIMBOT,
	__TAB_MISC,
	__TAB_VISUALS,
	__TAB_EXPLOITS,
	__TAB_CONFIG,
	__TAB_COUNT,
	__TAB_MAX = __TAB_COUNT - 1,
};

const char* g_arrTabNames[] = {"ESP", "CHAMS", "AIMBOT", "MISC", "VISUALS", "EXPLOITS", "CONFIG"};

class InputHandler : public Pendelum3::Pendelum3Input
{
public:
	virtual bool IsKeyDown(unsigned long key) { return g_pInput->Down(key); }
	virtual bool IsKeyPressed(unsigned long key) { return g_pInput->Pressed(key); }
	virtual bool IsKeyToggled(unsigned long key) { return g_pInput->Toggled(key); }
	virtual int GetKeyPressed() {
		for (int i = 0; i < 255; i++)
		{
			if (IsKeyPressed(i))
			{
				return i;
			}
		}
		return NULL;
	}

	virtual Pendelum3::PendelumCoords GetMousePosition()
	{
		if (!g_pInput->IsActiveWindow())
			return m_pcLastCoords;

		POINT p;
		if (GetCursorPos(&p))
		{
			if (ScreenToClient(g_pInput->GetLastWindowHandle(), &p))
			{
				m_pcLastCoords = { static_cast<float>(p.x), static_cast<float>(p.y) };
			}
		}
		return m_pcLastCoords;
	}



	Pendelum3::PendelumCoords m_pcLastCoords;
};


#define PENDELUMCOORDSTOCOORD2D(COORDS) {(COORDS).x, (COORDS).y}

class CSGOCheckBox : public Pendelum3::Pendelum3ValueItem<bool>
{
public:
	CSGOCheckBox(const char* szText, bool* pValue)
	{
		m_pValue = pValue;
		strcpy_s(m_szText, MAX_PATH, (char*)szText);
	}

	virtual void Render()
	{
		Pendelum3::PendelumCoordSet pcsCoords = m_pContainer->ContainerGetCoords();
		if (!ClipCoordSet(pcsCoords))
			return;

		// Check if Hovered

		std::array<int, 4> arrTextColor = { m_nRed, m_nGreen, m_nBlue, m_nAlpha };

		bool bInBounds = Pendelum3::g_pPendInput->MouseWithinPosition(pcsCoords);
		if (bInBounds || *m_pValue)
		{
			// Highlight If Hovered
			Pendelum3::g_pRenderer->Draw2DRectangle(PENDELUMCOORDSTOCOORD2D(pcsCoords.m_Min), PENDELUMCOORDSTOCOORD2D(pcsCoords.m_Max), { m_nRed / 2, m_nGreen / 2, m_nBlue / 2, m_nAlpha / 2 });
			arrTextColor = { 255,255,255,255 };
			if (bInBounds && Pendelum3::g_pPendInput->IsKeyToggled(VK_LBUTTON))
			{
				*m_pValue = !*m_pValue;
			}
		}


		// Center Text 
		Pendelum3::PendelumCoordSet textCoords = pcsCoords;
		std::pair<int, int> pTextSize = Pendelum3::g_pRenderer->GetTextSize(0, m_szText);
		Pendelum3::PendelumCoordSet textSize = { pTextSize.first, pTextSize.second };
		textCoords.m_Min = (((pcsCoords.m_Max - pcsCoords.m_Min) / 2.f) - ((textSize.m_Max - textSize.m_Min) / 2.f)) + pcsCoords.m_Min;
		textCoords.m_Min.x = max(pcsCoords.m_Min.x, textCoords.m_Min.x);
		textCoords.m_Min.y = max(pcsCoords.m_Min.y, textCoords.m_Min.y);

		if (!ClipCoordSet(pcsCoords))
			return;

		Pendelum3::g_pRenderer->_DrawText(0,
			PENDELUMCOORDSTOCOORD2D(textCoords.m_Min),
			PENDELUMCOORDSTOCOORD2D(textCoords.m_Max),
			m_szText,
			0,
			arrTextColor);


		Pendelum3::g_pRenderer->Draw2DLine({ pcsCoords.m_Min.x, pcsCoords.m_Min.y }, { pcsCoords.m_Min.x, pcsCoords.m_Max.y }, { m_nRed, m_nGreen, m_nBlue, m_nAlpha });
		Pendelum3::g_pRenderer->Draw2DLine({ pcsCoords.m_Max.x, pcsCoords.m_Max.y }, { pcsCoords.m_Min.x, pcsCoords.m_Max.y }, { m_nRed, m_nGreen, m_nBlue, m_nAlpha });
		Pendelum3::g_pRenderer->Draw2DLine({ pcsCoords.m_Max.x, pcsCoords.m_Max.y }, { pcsCoords.m_Max.x, pcsCoords.m_Min.y }, { m_nRed, m_nGreen, m_nBlue, m_nAlpha });
		Pendelum3::g_pRenderer->Draw2DLine({ pcsCoords.m_Min.x, pcsCoords.m_Min.y }, { pcsCoords.m_Max.x, pcsCoords.m_Min.y }, { m_nRed, m_nGreen, m_nBlue, m_nAlpha });
	}

private:
};

//template <class A> // Apparently we can't template this. Gay.
class CSGOFloatSlider : public Pendelum3::Pendelum3ValueItem<float>
{
public:
	CSGOFloatSlider(const char* szText, float* pValue, float Min, float Max)
	{
		m_pValue = pValue;
		strcpy_s(m_szText, MAX_PATH, (char*)szText);
		m_ValMin = Min;
		m_ValMax = Max;
	}


	float round(float var)
	{  // https://www.geeksforgeeks.org/rounding-floating-point-number-two-decimal-places-c-c/
		// 37.66666 * 100 =3766.66
		// 3766.66 + .5 =3767.16    for rounding off value
		// then type cast to int so value is 3767
		// then divided by 100 so the value converted into 37.67
		float value = (int)(var * 100 + .5);
		return (float)value / 100;
	}

	void Render()
	{
		Pendelum3::PendelumCoordSet pcsCoords = m_pContainer->ContainerGetCoords();
		if (!ClipCoordSet(pcsCoords))
			return;

		Pendelum3::g_pRenderer->Draw2DLine({ pcsCoords.m_Min.x, pcsCoords.m_Min.y }, { pcsCoords.m_Min.x, pcsCoords.m_Max.y }, { m_nRed, m_nGreen, m_nBlue, m_nAlpha });
		Pendelum3::g_pRenderer->Draw2DLine({ pcsCoords.m_Max.x, pcsCoords.m_Max.y }, { pcsCoords.m_Min.x, pcsCoords.m_Max.y }, { m_nRed, m_nGreen, m_nBlue, m_nAlpha });
		Pendelum3::g_pRenderer->Draw2DLine({ pcsCoords.m_Max.x, pcsCoords.m_Max.y }, { pcsCoords.m_Max.x, pcsCoords.m_Min.y }, { m_nRed, m_nGreen, m_nBlue, m_nAlpha });
		Pendelum3::g_pRenderer->Draw2DLine({ pcsCoords.m_Min.x, pcsCoords.m_Min.y }, { pcsCoords.m_Max.x, pcsCoords.m_Min.y }, { m_nRed, m_nGreen, m_nBlue, m_nAlpha });


		std::array<int, 4> arrTextColor = { m_nRed, m_nGreen, m_nBlue, m_nAlpha };

		bool bInBounds = Pendelum3::g_pPendInput->MouseWithinPosition(pcsCoords);
		Pendelum3::PendelumCoords pcMouseCoords = Pendelum3::g_pPendInput->GetMousePosition();
		if (bInBounds)
		{
			// Highlight If Hovered
			Pendelum3::g_pRenderer->Draw2DRectangle(PENDELUMCOORDSTOCOORD2D(pcsCoords.m_Min), PENDELUMCOORDSTOCOORD2D(pcsCoords.m_Max), { m_nRed / 2, m_nGreen / 2, m_nBlue / 2, m_nAlpha / 2 });
			arrTextColor = { 255,255,255,255 };
			if (bInBounds && Pendelum3::g_pPendInput->IsKeyDown(VK_LBUTTON))
			{
				*m_pValue = (pcMouseCoords.x - pcsCoords.m_Min.x) / (pcsCoords.m_Max.x - pcsCoords.m_Min.x);
				*m_pValue = std::clamp(*m_pValue, m_ValMin, m_ValMax);
				*m_pValue = round(*m_pValue);
			}
		}

		char szText[MAX_PATH];

		snprintf(szText, MAX_PATH, m_szText, *m_pValue); // Lets just hope you don't spam %n%n%n%n lol

		// Center Text 
		Pendelum3::PendelumCoordSet textCoords = pcsCoords;
		std::pair<int, int> pTextSize = Pendelum3::g_pRenderer->GetTextSize(0, szText);
		Pendelum3::PendelumCoordSet textSize = { pTextSize.first, pTextSize.second };
		textCoords.m_Min = (((pcsCoords.m_Max - pcsCoords.m_Min) / 2.f) - ((textSize.m_Max - textSize.m_Min) / 2.f)) + pcsCoords.m_Min;
		textCoords.m_Min.x = max(pcsCoords.m_Min.x, textCoords.m_Min.x);
		textCoords.m_Min.y = max(pcsCoords.m_Min.y, textCoords.m_Min.y);

		if (!ClipCoordSet(pcsCoords))
			return;

		Pendelum3::g_pRenderer->_DrawText(0,
			PENDELUMCOORDSTOCOORD2D(textCoords.m_Min),
			PENDELUMCOORDSTOCOORD2D(textCoords.m_Max),
			szText,
			0,
			arrTextColor);


		Pendelum3::PendelumCoordSet pcsSlider = pcsCoords;
		float flSize = ((pcsCoords.m_Max.x - pcsCoords.m_Min.x) * .1);
		pcsSlider.m_Min.x = ((*m_pValue * (pcsCoords.m_Max.x - pcsCoords.m_Min.x)) - (flSize/2.f)) + pcsCoords.m_Min.x;
		pcsSlider.m_Max.x = pcsSlider.m_Min.x + flSize;

		pcsSlider.m_Min.x = max(pcsSlider.m_Min.x, pcsCoords.m_Min.x);
		pcsSlider.m_Max.x = min(pcsSlider.m_Max.x, pcsCoords.m_Max.x);


		Pendelum3::g_pRenderer->Draw2DRectangle(PENDELUMCOORDSTOCOORD2D(pcsSlider.m_Min), PENDELUMCOORDSTOCOORD2D(pcsSlider.m_Max), arrTextColor);
	}
private:
};


class CSGOColorPicker : public Pendelum3::Pendelum3ValueItem<ColorToggle>
{
public:
	CSGOColorPicker() { Init(); }
	CSGOColorPicker(ColorToggle* pToggle) { m_pValue = pToggle; Init(); }

	void Init()
	{
		m_nRed = 52 * 2;
		m_nGreen = 113 * 2;
		m_nBlue = 235 * 2;
		m_nAlpha = 240 / 2;

	}

	void DrawBorder(Pendelum3::PendelumCoordSet pcsCoords)
	{
		Pendelum3::g_pRenderer->Draw2DLine({ pcsCoords.m_Min.x, pcsCoords.m_Min.y }, { pcsCoords.m_Min.x, pcsCoords.m_Max.y }, { m_nRed, m_nGreen, m_nBlue, m_nAlpha });
		Pendelum3::g_pRenderer->Draw2DLine({ pcsCoords.m_Max.x, pcsCoords.m_Max.y }, { pcsCoords.m_Min.x, pcsCoords.m_Max.y }, { m_nRed, m_nGreen, m_nBlue, m_nAlpha });
		Pendelum3::g_pRenderer->Draw2DLine({ pcsCoords.m_Max.x, pcsCoords.m_Max.y }, { pcsCoords.m_Max.x, pcsCoords.m_Min.y }, { m_nRed, m_nGreen, m_nBlue, m_nAlpha });
		Pendelum3::g_pRenderer->Draw2DLine({ pcsCoords.m_Min.x, pcsCoords.m_Min.y }, { pcsCoords.m_Max.x, pcsCoords.m_Min.y }, { m_nRed, m_nGreen, m_nBlue, m_nAlpha });
	}

	// https://www.geeksforgeeks.org/check-whether-a-given-point-lies-inside-a-triangle-or-not/
	float area(int x1, int y1, int x2, int y2, int x3, int y3)
	{
		return abs((x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2)) / 2.0);
	}

	/* A function to check whether point P(x, y) lies inside the triangle formed
	   by A(x1, y1), B(x2, y2) and C(x3, y3) */
	bool isInside(int x1, int y1, int x2, int y2, int x3, int y3, int x, int y)
	{
		/* Calculate area of triangle ABC */
		float A = area(x1, y1, x2, y2, x3, y3);

		/* Calculate area of triangle PBC */
		float A1 = area(x, y, x2, y2, x3, y3);

		/* Calculate area of triangle PAC */
		float A2 = area(x1, y1, x, y, x3, y3);

		/* Calculate area of triangle PAB */
		float A3 = area(x1, y1, x2, y2, x, y);

		/* Check if sum of A1, A2 and A3 is same as A */
		return (A == A1 + A2 + A3);
	}

	virtual void Render()
	{
		Pendelum3::PendelumCoordSet pcsCoords = m_pContainer->ContainerGetCoords();
		if (!ClipCoordSet(pcsCoords))
			return;

#if 1

		DrawBorder(pcsCoords);

		Pendelum3::PendelumCoordSet pcsCoordsSave = pcsCoords;
		Pendelum3::PendelumCoords diff = (pcsCoords.m_Max - pcsCoords.m_Min) * 0.002f;
		pcsCoords.m_Max -= diff;
		pcsCoords.m_Min += diff;
		pcsCoords.m_Max.x -= diff.x * .5;
		Pendelum3::g_pRenderer->Draw2DTriangle(
			{ ((pcsCoords.m_Max.x - pcsCoords.m_Min.x) / 2.f) + pcsCoords.m_Min.x, pcsCoords.m_Min.y },
			{ pcsCoords.m_Min.x, pcsCoords.m_Max.y },
			PENDELUMCOORDSTOCOORD2D(pcsCoords.m_Max),
			{ m_pValue->color.r(), m_pValue->color.g(), m_pValue->color.b(),255 }, 
			{ 255,255,255,255 },
			{ 0,0,0,255 }
		);

		if (Pendelum3::g_pPendInput->MouseWithinPosition(m_pcsCoords))
		{
			Pendelum3::PendelumCoords pcMousePos = Pendelum3::g_pPendInput->GetMousePosition();

			if (isInside(
				((pcsCoords.m_Max.x - pcsCoords.m_Min.x) / 2.f) + pcsCoords.m_Min.x,
				pcsCoords.m_Min.y,
				pcsCoords.m_Min.x,
				pcsCoords.m_Max.y,
				pcsCoords.m_Max.x,
				pcsCoords.m_Max.y,
				pcMousePos.x,
				pcMousePos.y))
			{
#if 0
				Pendelum3::g_pRenderer->Draw2DRectangle(
					PENDELUMCOORDSTOCOORD2D(pcMousePos - 3),
					PENDELUMCOORDSTOCOORD2D(pcMousePos + 3),
					{ 0,0,0,255 }
				);
				Pendelum3::g_pRenderer->Draw2DRectangle(
					PENDELUMCOORDSTOCOORD2D(pcMousePos - 2),
					PENDELUMCOORDSTOCOORD2D(pcMousePos + 2),
					{ 255,255,255,255 }
				);
#endif

				if (Pendelum3::g_pPendInput->IsKeyDown(VK_LBUTTON))
				{
					m_pcLastClickPos = pcMousePos;
				}
			}
		}

		if (isInside(
			((pcsCoords.m_Max.x - pcsCoords.m_Min.x) / 2.f) + pcsCoords.m_Min.x,
			pcsCoords.m_Min.y,
			pcsCoords.m_Min.x,
			pcsCoords.m_Max.y,
			pcsCoords.m_Max.x,
			pcsCoords.m_Max.y,
			m_pcLastClickPos.x,
			m_pcLastClickPos.y))
		{
			Pendelum3::g_pRenderer->Draw2DRectangle(
				PENDELUMCOORDSTOCOORD2D(m_pcLastClickPos - 3),
				PENDELUMCOORDSTOCOORD2D(m_pcLastClickPos + 3),
				{ 0,0,0,255 }
			);
			Pendelum3::g_pRenderer->Draw2DRectangle(
				PENDELUMCOORDSTOCOORD2D(m_pcLastClickPos - 2),
				PENDELUMCOORDSTOCOORD2D(m_pcLastClickPos + 2),
				{ 255,255,255,255 }
			);
		}


		Pendelum3::PendelumCoordSet pcsPickerCoords = pcsCoords;
		pcsCoords = pcsCoordsSave;


		Pendelum3::PendelumCoordSet pcsColorPicker;
		pcsColorPicker.m_Min.x = pcsPickerCoords.m_Max.x + 2;
		pcsColorPicker.m_Min.y = pcsPickerCoords.m_Min.y;
		pcsColorPicker.m_Max.x = pcsCoords.m_Max.x - 2;
		pcsColorPicker.m_Max.y = pcsPickerCoords.m_Max.y;

		Pendelum3::PendelumCoords Size = { 0, ((pcsPickerCoords.m_Max.y - pcsPickerCoords.m_Min.y) / 3.0) };

		Pendelum3::g_pRenderer->Draw2DRectangle(
			PENDELUMCOORDSTOCOORD2D(pcsColorPicker.m_Min),
			PENDELUMCOORDSTOCOORD2D(pcsColorPicker.m_Max - (Size * 2.0)),
			{ 255,0,0,255 },
			{ 0,255,0,255 },
			{ 0,255,0,255 },
			{ 255,0,0,255 }
		);
		Pendelum3::g_pRenderer->Draw2DRectangle(
			PENDELUMCOORDSTOCOORD2D(pcsColorPicker.m_Min + Size),
			PENDELUMCOORDSTOCOORD2D(pcsColorPicker.m_Max - Size),
			{ 0,255,0,255 },
			{ 0,0,255,255 },
			{ 0,0,255,255 },
			{ 0,255,0,255 }
		);
		Pendelum3::g_pRenderer->Draw2DRectangle(
			PENDELUMCOORDSTOCOORD2D(pcsColorPicker.m_Min + (Size * 2.0)),
			PENDELUMCOORDSTOCOORD2D(pcsColorPicker.m_Max),
			{ 0,0,255,255 },
			{ 255,0,0,255 },
			{ 255,0,0,255 },
			{ 0,0,255,255 }
		);
#endif

#if 0
		Pendelum3::g_pRenderer->Draw2DRectangle(
			PENDELUMCOORDSTOCOORD2D(pcsCoords.m_Min),
			PENDELUMCOORDSTOCOORD2D(pcsCoords.m_Max),
			{ 255, 255, 255, 255 },
			{ 0 ,0,0,255 },
			{ 0,0,0,255 }, 
			{ m_pValue->color.r(), m_pValue->color.g(), m_pValue->color.b(),255 }
		);
#endif

	}


private:
	Pendelum3::PendelumCoords m_pcPickerSize;
	Pendelum3::PendelumCoords m_pcLastClickPos;
};

class CSGOCombo : public Pendelum3::Pendelum3ValueItem<std::string>
{
public:
	CSGOCombo(std::vector <std::string>* pOpts)
	{ 
		m_pvecPosibleVals = pOpts;
		Init(); 
	}
	CSGOCombo(std::vector <std::string>* pOpts, std::string* pVal)
	{
		m_pvecPosibleVals = pOpts;
		m_pValue = pVal; 
		Init(); 
	}

	void Init()
	{
		m_nRed = 52 * 2;
		m_nGreen = 113 * 2;
		m_nBlue = 235 * 2;
		m_nAlpha = 240 / 2;

	}

	void DrawBorder(Pendelum3::PendelumCoordSet pcsCoords)
	{
		Pendelum3::g_pRenderer->Draw2DLine({ pcsCoords.m_Min.x, pcsCoords.m_Min.y }, { pcsCoords.m_Min.x, pcsCoords.m_Max.y }, { m_nRed, m_nGreen, m_nBlue, m_nAlpha });
		Pendelum3::g_pRenderer->Draw2DLine({ pcsCoords.m_Max.x, pcsCoords.m_Max.y }, { pcsCoords.m_Min.x, pcsCoords.m_Max.y }, { m_nRed, m_nGreen, m_nBlue, m_nAlpha });
		Pendelum3::g_pRenderer->Draw2DLine({ pcsCoords.m_Max.x, pcsCoords.m_Max.y }, { pcsCoords.m_Max.x, pcsCoords.m_Min.y }, { m_nRed, m_nGreen, m_nBlue, m_nAlpha });
		Pendelum3::g_pRenderer->Draw2DLine({ pcsCoords.m_Min.x, pcsCoords.m_Min.y }, { pcsCoords.m_Max.x, pcsCoords.m_Min.y }, { m_nRed, m_nGreen, m_nBlue, m_nAlpha });
	}

	virtual void Render()
	{

		Pendelum3::PendelumCoordSet pcsCoords = m_pContainer->ContainerGetCoords();
		if (!ClipCoordSet(pcsCoords))
			return;

		DrawBorder(pcsCoords);

		std::array<int, 4> arrTextColor = { m_nRed, m_nGreen, m_nBlue, m_nAlpha };









	}

private:
	std::vector <std::string>* m_pvecPosibleVals;
};

class CSGOColorPickerSimple : public Pendelum3::Pendelum3ValueItem<ColorToggle>, Pendelum3::Pendelum3Container
{
public:
	CSGOColorPickerSimple(const char* szName, ColorToggle* pValue)
	{
		const char* Vals[] = { "R : %f", "G : %f", "B : %f", "A : %f" };
		for (int i = 0; i < 4; i++)
		{
			m_arrSlider[i] = new CSGOFloatSlider(Vals[i], (float*)&(pValue->color->at(i)), 0.f,1.f);
			m_arrSlider[i]->SetContainer(this);
			m_arrSlider[i]->SetSize({ 150,30 });
		}	


		m_pCheckBox = new CSGOCheckBox("Enabled", &(pValue->bEnabled));
		m_pCheckBox->SetContainer(this);
		m_pValue = pValue;
	}
	

	virtual Pendelum3::PendelumCoordSet ContainerGetCoords()
	{
		return m_pcsCoords;
	}

	void Render()
	{
		Pendelum3::PendelumCoordSet pcsCoords = m_pContainer->ContainerGetCoords();
		if (!ClipCoordSet(pcsCoords))
			return;

		float flCurrPosY = 0;
		m_pCheckBox->SetSize({ 150.f, 20.f });
		m_pCheckBox->SetColor(m_nRed, m_nGreen, m_nBlue, 240);
		m_pCheckBox->SetRelativeCoordinates(2.f, flCurrPosY);
		m_pCheckBox->Render();
		flCurrPosY += m_pCheckBox->GetSize().y + 2.f;


		for (int i = 0; i < 4; i++)
		{
			m_arrSlider[i]->SetSize({ 150,20 });
			m_arrSlider[i]->SetColor(m_nRed, m_nGreen, m_nBlue, 240);
			m_arrSlider[i]->SetRelativeCoordinates(2.f, flCurrPosY);
			m_arrSlider[i]->Render();
			flCurrPosY += m_arrSlider[i]->GetSize().y + 2.f;
		}


		
		Pendelum3::PendelumCoordSet pcsColorShow;
		pcsColorShow = pcsCoords;
		pcsColorShow.m_Min.x += 150.f + 5.f;
		pcsColorShow.m_Max.x -= 3.f;
		pcsColorShow.m_Min.y += 3.f;
		pcsColorShow.m_Max.y = pcsCoords.m_Min.y + flCurrPosY - 4.f;

		//if (!ClipCoordSet(pcsColorShow))
		//	return;

		if (m_pValue)
		{
			Pendelum3::g_pRenderer->Draw2DRectangle(
				PENDELUMCOORDSTOCOORD2D(pcsColorShow.m_Min),
				PENDELUMCOORDSTOCOORD2D(pcsColorShow.m_Max),
				{ (int)(m_pValue->color->at(0) * 255),(int)(m_pValue->color->at(1) * 255),(int)(m_pValue->color->at(2) * 255),(int)(m_pValue->color->at(3) * 255) });
		}


		for (int i = 0; i < 2; i++)
		{
			pcsColorShow.m_Min += 1.0f;
			pcsColorShow.m_Max -= 1.0f;
			Pendelum3::g_pRenderer->Draw2DLine({ pcsColorShow.m_Min.x, pcsColorShow.m_Min.y }, { pcsColorShow.m_Min.x, pcsColorShow.m_Max.y }, { m_nRed, m_nGreen, m_nBlue, 255 });
			Pendelum3::g_pRenderer->Draw2DLine({ pcsColorShow.m_Max.x, pcsColorShow.m_Max.y }, { pcsColorShow.m_Min.x, pcsColorShow.m_Max.y }, { m_nRed, m_nGreen, m_nBlue, 255 });
			Pendelum3::g_pRenderer->Draw2DLine({ pcsColorShow.m_Max.x, pcsColorShow.m_Max.y }, { pcsColorShow.m_Max.x, pcsColorShow.m_Min.y }, { m_nRed, m_nGreen, m_nBlue, 255 });
			Pendelum3::g_pRenderer->Draw2DLine({ pcsColorShow.m_Min.x, pcsColorShow.m_Min.y }, { pcsColorShow.m_Max.x, pcsColorShow.m_Min.y }, { m_nRed, m_nGreen, m_nBlue, 255 });

		}

		Pendelum3::g_pRenderer->Draw2DLine({ pcsColorShow.m_Min.x, pcsColorShow.m_Min.y }, { pcsColorShow.m_Min.x, pcsColorShow.m_Max.y }, { 0, 0, 0, 255 });
		Pendelum3::g_pRenderer->Draw2DLine({ pcsColorShow.m_Max.x, pcsColorShow.m_Max.y }, { pcsColorShow.m_Min.x, pcsColorShow.m_Max.y }, { 0, 0, 0, 255 });
		Pendelum3::g_pRenderer->Draw2DLine({ pcsColorShow.m_Max.x, pcsColorShow.m_Max.y }, { pcsColorShow.m_Max.x, pcsColorShow.m_Min.y }, { 0, 0, 0, 255 });
		Pendelum3::g_pRenderer->Draw2DLine({ pcsColorShow.m_Min.x, pcsColorShow.m_Min.y }, { pcsColorShow.m_Max.x, pcsColorShow.m_Min.y }, { 0, 0, 0, 255 });

		pcsColorShow.m_Min += 1.0f;
		pcsColorShow.m_Max -= 1.0f;

		Pendelum3::g_pRenderer->Draw2DLine({ pcsColorShow.m_Min.x, pcsColorShow.m_Min.y }, { pcsColorShow.m_Min.x, pcsColorShow.m_Max.y }, { 0, 0, 0, 255 });
		Pendelum3::g_pRenderer->Draw2DLine({ pcsColorShow.m_Max.x, pcsColorShow.m_Max.y }, { pcsColorShow.m_Min.x, pcsColorShow.m_Max.y }, { 0, 0, 0, 255 });
		Pendelum3::g_pRenderer->Draw2DLine({ pcsColorShow.m_Max.x, pcsColorShow.m_Max.y }, { pcsColorShow.m_Max.x, pcsColorShow.m_Min.y }, { 0, 0, 0, 255 });
		Pendelum3::g_pRenderer->Draw2DLine({ pcsColorShow.m_Min.x, pcsColorShow.m_Min.y }, { pcsColorShow.m_Max.x, pcsColorShow.m_Min.y }, { 0, 0, 0, 255 });

		Pendelum3::g_pRenderer->Draw2DLine({ pcsCoords.m_Min.x, pcsCoords.m_Min.y }, { pcsCoords.m_Min.x, pcsCoords.m_Max.y }, { m_nRed, m_nGreen, m_nBlue, m_nAlpha });
		Pendelum3::g_pRenderer->Draw2DLine({ pcsCoords.m_Max.x, pcsCoords.m_Max.y }, { pcsCoords.m_Min.x, pcsCoords.m_Max.y }, { m_nRed, m_nGreen, m_nBlue, m_nAlpha });
		Pendelum3::g_pRenderer->Draw2DLine({ pcsCoords.m_Max.x, pcsCoords.m_Max.y }, { pcsCoords.m_Max.x, pcsCoords.m_Min.y }, { m_nRed, m_nGreen, m_nBlue, m_nAlpha });
		Pendelum3::g_pRenderer->Draw2DLine({ pcsCoords.m_Min.x, pcsCoords.m_Min.y }, { pcsCoords.m_Max.x, pcsCoords.m_Min.y }, { m_nRed, m_nGreen, m_nBlue, m_nAlpha });
	}

private:
	CSGOFloatSlider* m_arrSlider[4];
	CSGOCheckBox* m_pCheckBox;
};

class CSGOMenu : public Pendelum3::Pendelum3Window // Over All Menu
{
public:
	CSGOMenu(Pendelum3::PendelumCoordSet pcsCoords, char* sWindowTitle)
	{
		m_pcsCoords = pcsCoords;
		RecalculateSize();
		strcpy_s(m_sWindowName, sizeof(m_sWindowName), sWindowTitle);
	}
	CSGOMenu(float x, float y, float x2, float y2, char* sWindowTitle)
	{
		m_pcsCoords = { x,y,x2,y2 };
		RecalculateSize();
		strcpy_s(m_sWindowName, sizeof(m_sWindowName), sWindowTitle);
	}
	CSGOMenu(float x, float y, char* sWindowTitle)
	{
		m_pcsCoords.m_Min = { x,y };
		strcpy_s(m_sWindowName, sizeof(m_sWindowName), sWindowTitle);
	}
	CSGOMenu(float x, float y, Pendelum3::PendelumCoords pcSize, char* sWindowTitle)
	{
		m_pcsCoords.m_Min = { x,y };
		m_pcSize = pcSize;
		RecalculateCoordinates();
		strcpy_s(m_sWindowName, sizeof(m_sWindowName), sWindowTitle);
	}

	void PrintInfo()
	{
		VCON("Rendering Menu! { %f %f } -> { %f %f } : Size { %f %f } : Padding { %f %f } \n", m_pcsCoords.m_Min.x, m_pcsCoords.m_Min.y, m_pcsCoords.m_Max.x, m_pcsCoords.m_Max.y, m_pcSize.x, m_pcSize.y, m_pcMemberPadding.x, m_pcMemberPadding.y);
	}

	virtual void Render() {
		// Draw Background
		m_pcCursorPos = { 0,0 };
		double dbCurrentTime = GetFloatTime();
		if ((m_dbAnimationTimer + m_dbAnimationTime) > dbCurrentTime)
		{
			float m_Divisable = static_cast<float>((dbCurrentTime - m_dbAnimationTimer) / m_dbAnimationTime);
			if (!m_bWasOpen)
				m_Divisable = 1.0f - m_Divisable;
			Pendelum3::g_pRenderer->Draw2DRectangleFadeMiddle(
				{ m_pcsCoords.m_Min.x, m_pcsCoords.m_Min.y }, 
				{ m_pcSize.x, m_pcSize.y }, 
				std::array<int, 4>{ max(m_nRed - 10, 0), 0, max(m_nBlue - 10, 0), max(static_cast<int>(static_cast<float>(220)* m_Divisable) + 20, 0) },
				std::array<int,4>{ m_nRed, m_nGreen, m_nBlue, static_cast<int>(static_cast<float>(m_nAlpha) * m_Divisable) }
			);
			return;
		}
		else {
			Pendelum3::g_pRenderer->Draw2DRectangleFadeMiddle(
				{ m_pcsCoords.m_Min.x, m_pcsCoords.m_Min.y }, 
				{ m_pcSize.x, m_pcSize.y }, 
				{ max(m_nRed - 10, 0), max(m_nGreen - 10, 0), max(m_nBlue - 20, 0), 230 },
				{ m_nRed, m_nGreen, m_nBlue, m_nAlpha }
			);
		}
		// Draw Title Bar
		const char* szText = XorStr("GoldenGun v.1");
		auto size = g_pRenderer->GetTextSize(0, szText);
		float xpos = 5;
		float ypos = 0;
		g_pRenderer->_DrawText(0, { xpos + 2.f,ypos + 2.f }, { float{xpos + size.first + 2.f }, float{ ypos + size.second + 2.f } }, (char*)szText, strlen((char*)szText), { 0,0,0,255 });
		g_pRenderer->_DrawText(0, { xpos + 1.f,ypos + 1.f }, { float{xpos + size.first + 1.f }, float{ ypos + size.second + 1.f} }, (char*)szText, strlen((char*)szText), { 235/2, 201/2,52/2,255 });
		g_pRenderer->_DrawText(0, { xpos,ypos }, { float{xpos + size.first }, float{ ypos + size.second } }, (char*)szText, strlen((char*)szText), { 235, 201, 52,255 });

		szText = XorStr("By Enron");
		size = g_pRenderer->GetTextSize(0, szText);
		xpos = (g_GRScreenX - size.first) - 10;
		ypos = 0;
		g_pRenderer->_DrawText(0, { xpos + 2.f,ypos + 2.f }, { float{xpos + size.first + 2.f }, float{ ypos + size.second + 2.f } }, (char*)szText, strlen((char*)szText), { 0,0,0,255 });
		g_pRenderer->_DrawText(0, { xpos + 1.f,ypos + 1.f }, { float{xpos + size.first + 1.f }, float{ ypos + size.second + 1.f} }, (char*)szText, strlen((char*)szText), { 235 / 2, 201 / 2,52 / 2,255 });
		g_pRenderer->_DrawText(0, { xpos,ypos }, { float{xpos + size.first }, float{ ypos + size.second } }, (char*)szText, strlen((char*)szText), { 235, 201, 52,255 });

		// Draw Menu Items
		for (Pendelum3::Pendelum3MenuItem*& pMenuItem : m_vMenuItems)
		{
#ifndef _DEBUG // Release optimization options cause issues if I don't do this
			pMenuItem->SetContainer(this);
#endif
			pMenuItem->UpdateCoords();
			pMenuItem->UpdateSize();
			pMenuItem->Render();
		}
	}

	void OnClose()
	{
		m_bWasOpen = false;
		m_dbAnimationTimer = GetFloatTime();
	}

	void OnOpen()
	{
		m_bWasOpen = true;
		m_dbAnimationTimer = GetFloatTime();
	}

	bool HasFinishedAnimation()
	{
		return ((m_dbAnimationTimer + m_dbAnimationTime) < GetFloatTime());
	}

private:
	bool m_bWasOpen = false;
	double m_dbAnimationTimer = 0.0;
	double m_dbAnimationTime = 0.5f;
};

class CSGOESPMenu : public Pendelum3::Pendelum3SubMenu
{
public:

	enum ESPTabs
	{
		__ESPTAB_ENEMY,
		__ESPTAB_TEAMMATE,
		__ESPTAB_PLAYERSPECIFIC,

		__ESP_COUNT,
		__ESP_MAX = __ESP_COUNT - 1,
	};

	CSGOESPMenu()
	{
		m_nRed = 52 * 2;
		m_nGreen = 113 * 2;
		m_nBlue = 235 * 2;
		m_nAlpha = 240 / 2;


		SetPadding(20, 20);
		const char* arrTabName[] = {"ENEMIES", "TEAMMATES", "PLAYER"};
		for (int i = 0; i < __ESP_COUNT; i++)
		{
			CSGOCheckBox* pCheckBox = new CSGOCheckBox(arrTabName[i], &(m_arrTabs[i]));
			pCheckBox->SetSize({ 100.f, 20.f });
			pCheckBox->SetCoordinates(200.f, 200.f);
			pCheckBox->SetColor(m_nRed * 2, m_nGreen * 2, m_nBlue * 2, 240);
			pCheckBox->SetContainer(this);
			m_vTabs.push_back(pCheckBox);
		}

	}

	void RenderSquare(Pendelum3::PendelumCoordSet& pcsCoords, std::array<int, 4> color)
	{
		Pendelum3::g_pRenderer->Draw2DLine({ pcsCoords.m_Min.x, pcsCoords.m_Min.y }, { pcsCoords.m_Min.x, pcsCoords.m_Max.y }, color);
		Pendelum3::g_pRenderer->Draw2DLine({ pcsCoords.m_Max.x, pcsCoords.m_Max.y }, { pcsCoords.m_Min.x, pcsCoords.m_Max.y }, color);
		Pendelum3::g_pRenderer->Draw2DLine({ pcsCoords.m_Max.x, pcsCoords.m_Max.y }, { pcsCoords.m_Max.x, pcsCoords.m_Min.y }, color);
		Pendelum3::g_pRenderer->Draw2DLine({ pcsCoords.m_Min.x, pcsCoords.m_Min.y }, { pcsCoords.m_Max.x, pcsCoords.m_Min.y }, color);
	}

	void RenderESPPreview(Config::ESP::PlayerESP* ESP)
	{
		// Draw ESP Square
		Pendelum3::PendelumCoordSet pcsCoords = GetCoords();
		pcsCoords.m_Max -= 50;
		pcsCoords.m_Min.y += 20;
		pcsCoords.m_Min.x = pcsCoords.m_Max.x - 300;
		pcsCoords.m_Max.y = pcsCoords.m_Min.y + 600;


		Pendelum3::PendelumCoordSet BoxCoords = pcsCoords;
		BoxCoords.m_Min -= 5;
		BoxCoords.m_Max += 5;
		Pendelum3::g_pRenderer->Draw2DRectangle(PENDELUMCOORDSTOCOORD2D(BoxCoords.m_Min), PENDELUMCOORDSTOCOORD2D(BoxCoords.m_Max), {150,150,150,255});

		if (ESP->ctBox.bEnabled)
		{
			std::array<int, 4> color = { int(ESP->ctBox.color.color.at(0) * 255),int(ESP->ctBox.color.color.at(1) * 255) ,int(ESP->ctBox.color.color.at(2) * 255) ,int(ESP->ctBox.color.color.at(3) * 255) };
			RenderSquare(pcsCoords, color);
			pcsCoords.m_Min.x -= 1;
			pcsCoords.m_Min.y -= 1;
			pcsCoords.m_Max.x += 2;
			pcsCoords.m_Max.y += 2;
			RenderSquare(pcsCoords, color);
			pcsCoords.m_Min.x -= 1;
			pcsCoords.m_Min.y -= 1;
			pcsCoords.m_Max.x += 2;
			pcsCoords.m_Max.y += 2;
			RenderSquare(pcsCoords, { 0,0,0,255 });
			pcsCoords.m_Min.x += 3;
			pcsCoords.m_Min.y += 3;
			pcsCoords.m_Max.x -= 4;
			pcsCoords.m_Max.y -= 4;
			RenderSquare(pcsCoords, { 0,0,0,255 });
		}

	}


	void RenderESPControls(Config::ESP::PlayerESP* ESP)
	{
		RenderESPPreview(ESP);

		static ColorToggle toggle = ESP->ctBox;
		static CSGOColorPickerSimple pickertest("Color", &toggle);
		toggle = ESP->ctBox;
		pickertest.SetColor(m_nRed * 2, m_nGreen * 2, m_nBlue * 2, m_nAlpha);
		pickertest.SetSize({ 200,110 });
		pickertest.SetRelativeCoordinates(150.f, 100.f);
		pickertest.SetContainer(this);
		pickertest.Render();
		ESP->ctBox = toggle;

		static ColorToggle toggle2 = ESP->ctName;
		static CSGOColorPickerSimple namepicker("Color", &toggle2);
		toggle2 = ESP->ctName;
		namepicker.SetColor(m_nRed * 2, m_nGreen * 2, m_nBlue * 2, m_nAlpha);
		namepicker.SetSize({ 200,110 });
		namepicker.SetRelativeCoordinates(150.f, 300.f);
		namepicker.SetContainer(this);
		namepicker.Render();
		ESP->ctName = toggle2;

	}

	virtual void Render()
	{
		m_pcCursorPos = { 0,0 };
		Pendelum3::PendelumCoordSet pcsCoords = m_pContainer->ContainerGetCoords();
		pcsCoords.m_Min += m_pContainer->GetDefaultMemberPadding();
		pcsCoords.m_Max -= m_pContainer->GetDefaultMemberPadding();
		Pendelum3::PendelumCoordSet pcsCoordsSave = m_pcsCoords;
		if (!ClipCoordSet(pcsCoords))
			return;

		m_pcsCoords = pcsCoords;
		RenderSquare(m_pcsCoords, { m_nRed, m_nGreen, m_nBlue, m_nAlpha });

		// DO NOT TOUCH THE FOLLOWING LINE OF CODE, EVER!
		m_pcCursorPos.x = ((((pcsCoords.m_Max.x - pcsCoords.m_Min.x) / 2.f)  + pcsCoords.m_Min.x) - ((105.f * 3.f) * 2.f)) + ((105.f * 3.f) / 2.f); 

		int nLargesty = 0;
		for (Pendelum3::Pendelum3MenuItem*& pTab : m_vTabs)
		{
			pTab->SetRelativeCoordinates(m_pcCursorPos.x, m_pcCursorPos.y);
			pTab->UpdateCoords();
			pTab->UpdateSize();
			pTab->Render();
			m_pcCursorPos.x += pTab->GetSize().x + 5;
			nLargesty = max(pTab->GetSize().y, nLargesty);
		}

		static int nLastTab = -1;
		for (int i = 0; i < __ESP_COUNT; i++)
		{
			if (m_arrTabs[i] && i != nLastTab)
			{
				nLastTab = i;
				for (int b = 0; b < __ESP_COUNT; b++)
				{
					m_arrTabs[b] = false;
				}
				m_arrTabs[i] = true;
				break;
			}
		}

		switch (nLastTab)
		{
		case __ESPTAB_ENEMY:
			RenderESPControls(&(g_pConfig->m_ESP.m_EnemyESP));
			break;
		case __ESPTAB_TEAMMATE:
			RenderESPControls(&(g_pConfig->m_ESP.m_TeammateESP));
			break;
		case __ESPTAB_PLAYERSPECIFIC:
			//RenderESPControls(&(g_pConfig->m_ESP.m_EnemyESP));
			break;
		};



		m_pcsCoords = pcsCoordsSave;
		return;
	}


private:

	std::vector<Pendelum3::Pendelum3MenuItem*> m_vTabs;
	bool m_arrTabs[__ESP_COUNT];

};



class CSGOSubMenu : public Pendelum3::Pendelum3SubMenu // Main Tab
{
public:
	CSGOSubMenu()
	{
		m_nRed = 52;
		m_nGreen = 113;
		m_nBlue = 235;
		m_nAlpha = 240;


		// Set Up Our Tabs

		for (int i = 0; i < __TAB_COUNT; i++)
		{
			CSGOCheckBox* pCheckBox = new CSGOCheckBox(g_arrTabNames[i], &(m_arrTabs[i]));
			pCheckBox->SetSize({ 100.f, 20.f });
			pCheckBox->SetCoordinates(200.f, 200.f);
			pCheckBox->SetColor(m_nRed * 2, m_nGreen * 2, m_nBlue * 2, 240);
			pCheckBox->SetContainer(this);
			m_vTabs.push_back(pCheckBox);
		}

		// Set Up Our Menus

		m_pESPMenu = new CSGOESPMenu;
		m_pESPMenu->SetContainer(this);
	}

	void PrintInfo()
	{
		VCON("Rendering Sub Menu! { %f %f } -> { %f %f } : Size { %f %f }\n", m_pcsCoords.m_Min.x, m_pcsCoords.m_Min.y, m_pcsCoords.m_Max.x, m_pcsCoords.m_Max.y, m_pcSize.x, m_pcSize.y);
	}

	void RenderBorder()
	{
		Pendelum3::g_pRenderer->Draw2DLine({ m_pcsCoords.m_Min.x, m_pcsCoords.m_Min.y }, { m_pcsCoords.m_Min.x, m_pcsCoords.m_Max.y }, { m_nRed, m_nGreen, m_nBlue, m_nAlpha });
		Pendelum3::g_pRenderer->Draw2DLine({ m_pcsCoords.m_Max.x, m_pcsCoords.m_Max.y }, { m_pcsCoords.m_Min.x, m_pcsCoords.m_Max.y }, { m_nRed, m_nGreen, m_nBlue, m_nAlpha });
		Pendelum3::g_pRenderer->Draw2DLine({ m_pcsCoords.m_Max.x, m_pcsCoords.m_Max.y }, { m_pcsCoords.m_Max.x, m_pcsCoords.m_Min.y }, { m_nRed, m_nGreen, m_nBlue, m_nAlpha });
		Pendelum3::g_pRenderer->Draw2DLine({ m_pcsCoords.m_Min.x, m_pcsCoords.m_Min.y }, { m_pcsCoords.m_Max.x, m_pcsCoords.m_Min.y }, { m_nRed, m_nGreen, m_nBlue, m_nAlpha });
	}

	virtual void Render()
	{
		m_pcCursorPos = { 0,0 };
		RenderBorder();


		// Render Out Tabs
		int nLargestx = 0;
		for (Pendelum3::Pendelum3MenuItem*& pTab : m_vTabs)
		{
			pTab->SetRelativeCoordinates(m_pcCursorPos.x, m_pcCursorPos.y);
			pTab->UpdateCoords();
			pTab->UpdateSize();
			pTab->Render();
			m_pcCursorPos.y += pTab->GetSize().y + 5;
			nLargestx = max(pTab->GetSize().x, nLargestx);
		}

		static int nLastTab = -1;
		for (int i = 0; i < __TAB_COUNT; i++)
		{
			if (m_arrTabs[i] && i != nLastTab)
			{
				nLastTab = i;
				for (int b = 0; b < __TAB_COUNT; b++)
				{
					m_arrTabs[b] = false;		
				}
				m_arrTabs[i] = true;
				break;
			}
		}

		m_pcCursorPos.x += nLargestx + 20;
		m_pcCursorPos.y = 0;
		switch (nLastTab)
		{
		case __TAB_ESP:
		{
			// m_pESPMenu->SetSize({ (GetSize() * .66f).x, (GetSize().y ) - m_pcMemberPadding.y}); <-- For when I can actually render the golden gun....
			m_pESPMenu->SetSize({ (GetSize() * .99f).x, (GetSize().y ) - m_pcMemberPadding.y});
			m_pESPMenu->SetRelativeCoordinates(m_pcCursorPos.x, m_pcCursorPos.y);
			m_pESPMenu->Render();
		}
			break;
		case __TAB_CHAMS:
			break;
		case __TAB_AIMBOT:
			break;
		case __TAB_EXPLOITS:
			break;
		default:
			break;
		}





#if 0 // Dynamic Rendering Probably Ain't the best for this sub-menu....
		for (Pendelum3::Pendelum3MenuItem*& pItem : m_vMenuItems)
		{
			pItem->SetRelativeCoordinates(m_pcCursorPos.x, m_pcCursorPos.y);
			pItem->UpdateCoords();
			pItem->UpdateSize();
			pItem->Render();
			m_pcCursorPos.y += pItem->GetSize().y + 5;
		}
#endif
	}
private:
	std::vector<Pendelum3::Pendelum3MenuItem*> m_vTabs;
	bool m_arrTabs[__TAB_COUNT];
	CSGOESPMenu* m_pESPMenu = nullptr;
};
class CSGOSubMenu* g_pMenuContainer = nullptr;


CSGOMenu* g_pCsgoMenu = nullptr;
void Menu::Define()
{
	
	float width, height;
	width = static_cast<float>(m_flpLastWindowSize.first) ;
	height = -static_cast<float>(m_flpLastWindowSize.second) ;
	g_pCsgoMenu = new CSGOMenu(width, height, XorStr("GoldenGun"));
	
	Pendelum3::Pendelum3Window* pMaster = g_pCsgoMenu;
	pMaster->SetCoordinates({ 0.f,0.f });
	pMaster->SetSize(width, height);

	pMaster->SetPadding(width * .05f, height * .05f);
	pMaster->RecalculateCoordinates();
	pMaster->SetCoordinates({ 0.f,0.f });
	pMaster->SetColor(2, 0, 31, 170);
	//pMaster->SetColor(4, 0, 62, 160);

	g_pMenuContainer = new CSGOSubMenu();
	width *= .90f;
	height *= .90f;
	g_pMenuContainer->PrintInfo();
	g_pMenuContainer->SetSize(width, height);
	g_pMenuContainer->PrintInfo();
	pMaster->AddMember(g_pMenuContainer);
	g_pMenuContainer->SetPadding(20, 20);
	g_pMenuContainer->PrintInfo();
	g_pMenuContainer->SetContainer(g_pCsgoMenu);

	m_pRenderInstance->AddWindow(pMaster);
}



void Menu::Init()
{
	m_pRenderInstance = new Pendelum3::Pendelum3RenderInstance();
	Pendelum3::g_pPendInput = new InputHandler;
	Define();
}

void Menu::OnPresent()
{
	if(m_bIsMenuOpen)
		g_pInput->UnLockInput();

	if (g_pInput->Toggled(VK_INSERT))
	{
		m_bIsMenuOpen = !m_bIsMenuOpen;
	}
}

bool Menu::HasAnimationFinished()
{
	return g_pCsgoMenu->HasFinishedAnimation();
}

void Menu::Render()
{
	g_pInput->UnLockInput();

	static bool bInit{ false };
	if (!bInit)
	{
		bInit = true;
		Init();
	}



	static bool bWasOpen{ false };
	static bool bMenuCloseTriggered{ false };
	if (!m_bIsMenuOpen)
	{
		if (!bMenuCloseTriggered) {
			g_pCsgoMenu->OnClose();
			bMenuCloseTriggered = true;
		}
		if (g_pCsgoMenu->HasFinishedAnimation())
		{
			if (bWasOpen)
			{
				bWasOpen = false;
				g_pInterfaces->m_pInputSysem->ResetInputState();
			}
			g_pInterfaces->m_pInputSysem->EnableInput(true);
			return;
		}
	}
	else if (!bWasOpen) {	
		bMenuCloseTriggered = false;
		if (g_pCsgoMenu->HasFinishedAnimation())
		{
			g_pCsgoMenu->OnOpen();
			g_pInterfaces->m_pInputSysem->EnableInput(false);
			bWasOpen = true;
		}
	}

	float width, height;
	width = static_cast<float>(m_flpLastWindowSize.first);
	height = -static_cast<float>(m_flpLastWindowSize.second);
	g_pCsgoMenu->SetSize(width, height);
	width *= .90f;
	height *= .90f;

	g_pMenuContainer->SetSize(width, height);
	m_pRenderInstance->Render();
	g_pInput->LockInput();
}




bool Menu::OnLockCursor()
{
	if (m_bIsMenuOpen)
	{
		g_pInterfaces->m_pSurface->UnlockCursor();
		return true;
	}
	return false;
}
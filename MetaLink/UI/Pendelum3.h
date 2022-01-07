#pragma once
#if 1
#include "../BaseIncludes.h"
#include "../FunctionTypeDeclares.h"
#else

#endif
#include "GoldenRenderer.h"
#define SimpleOperatorOverload(OP, VALTYPE) void operator##OP##=(const VALTYPE& other) { x OP##= static_cast<float>(other); y OP##= static_cast<float>(other); } \
		\
		 PendelumCoords operator##OP##(const VALTYPE& other) { PendelumCoords New; New.x = x OP static_cast<float>(other); New.y = y OP static_cast<float>(other); return New; }; 


namespace Pendelum3 {

	__forceinline float GetTime()
	{
		return static_cast<float>(((Plat_FloatTimeFunc_t)Plat_FloatTime)());
	}


	class Pendelum3RenderInstance;
	class Pendelum3Window;
	class Pendelum3SubMenu;
	struct PendelumCoords;
	class Pendelum3Object;
	class Pendelum3MenuItem;
	class Pendelum3Input;

	inline GoldenRenderer* g_pRenderer = NULL;
	inline Pendelum3Input* g_pPendInput = NULL;


	class Pendelum3RenderInstance
	{
	public:
		Pendelum3RenderInstance() { }
		void Render();
		void SetFocusWindow(Pendelum3Window* pWindow);
		void AddWindow(Pendelum3Window* pWindow) { m_vWindows.push_back(pWindow); }
	private:
		std::vector<Pendelum3Window*> m_vWindows;
		
	};



	struct PendelumCoords
	{
		SimpleOperatorOverload(*, float);
		SimpleOperatorOverload(+, float);
		SimpleOperatorOverload(/, float);
		SimpleOperatorOverload(- , float);
		SimpleOperatorOverload(*, double);
		SimpleOperatorOverload(+, double);
		SimpleOperatorOverload(/ , double);
		SimpleOperatorOverload(-, double);
		SimpleOperatorOverload(*, int);
		SimpleOperatorOverload(+, int);
		SimpleOperatorOverload(/ , int);
		SimpleOperatorOverload(-, int);

		void operator=(const float& val) { x = val; y = val; }
		void operator-=(const PendelumCoords& other) { x -= other.x; y -= other.y; }
		PendelumCoords operator -(const PendelumCoords& other) { PendelumCoords New; New.x = x - other.x; New.y = y - other.y; return New; }
		void operator+=(const PendelumCoords& other) { x += other.x; y += other.y; }
		PendelumCoords operator +(const PendelumCoords& other) { PendelumCoords New; New.x = x + other.x; New.y = y + other.y; return New; }
		void operator*=(const PendelumCoords& other) { x *= other.x; y *= other.y; }
		PendelumCoords operator *(const PendelumCoords& other) { PendelumCoords New; New.x = x * other.x; New.y = y * other.y; return New; }
		void operator/=(const PendelumCoords& other) { x /= other.x; y /= other.y; }
		PendelumCoords operator /(const PendelumCoords& other) { PendelumCoords New; New.x = x / other.x; New.y = y / other.y; return New; }
		float x, y = 0.f;
	};

	struct PendelumCoordSet
	{
		PendelumCoords m_Max;
		PendelumCoords m_Min;
	};

	class Pendelum3Input // Override with User Defined Class
	{
	public:
		virtual bool IsKeyDown(unsigned long) = 0;
		virtual bool IsKeyPressed(unsigned long) = 0;
		virtual bool IsKeyToggled(unsigned long key) = 0;
		virtual int GetKeyPressed() = 0;
		virtual Pendelum3::PendelumCoords GetMousePosition() = 0;
		virtual bool MouseWithinPosition(PendelumCoordSet pcsCoords)
		{
			Pendelum3::PendelumCoords pcMouse = GetMousePosition();
			if ((pcsCoords.m_Min.x < pcMouse.x) && (pcsCoords.m_Max.x > pcMouse.x) && (pcsCoords.m_Min.y < pcMouse.y) && (pcsCoords.m_Max.y > pcMouse.y))
				return true;			
			return false;
		}
	private:
	};




	class Pendelum3Object
	{
	public:
		virtual void Render() = 0;
		//void SetCoordinates(float& x, float& y, float& x2, float& y2) { m_pcsCoords.m_Min = { x,y }; m_pcsCoords.m_Max = { x2, y2 }; m_pcSize = m_pcsCoords.m_Max - m_pcsCoords.m_Min; }
		void SetCoordinates(float x, float y, float x2, float y2) { m_pcsCoords.m_Min = { x,y }; m_pcsCoords.m_Max = { x2, y2 }; m_pcSize = m_pcsCoords.m_Max - m_pcsCoords.m_Min; }
		//void SetCoordinates(float& x, float& y) { m_pcsCoords.m_Min = { x,y }; m_pcsCoords.m_Max = m_pcsCoords.m_Min + m_pcSize; }
		void SetCoordinates(float x, float y) { m_pcsCoords.m_Min = { x,y }; m_pcsCoords.m_Max = m_pcsCoords.m_Min + m_pcSize; }
		void SetCoordinates(PendelumCoordSet pcsNewCoords) { m_pcsCoords = pcsNewCoords; }
		void SetSize(float& x, float& y) { m_pcSize = { x,y }; m_pcsCoords.m_Max = m_pcsCoords.m_Min + m_pcSize; }
		//void SetSize(float x, float y) { m_pcSize = { x,y }; m_pcsCoords.m_Max = m_pcsCoords.m_Min + m_pcSize; }
		void SetSize(PendelumCoords pcSize) { m_pcSize = pcSize; }
		void RecalculateSize() { m_pcSize = m_pcsCoords.m_Max - m_pcsCoords.m_Min; }
		void RecalculateCoordinates() { m_pcsCoords.m_Max = m_pcsCoords.m_Min + m_pcSize; }
		PendelumCoordSet GetCoords() { return m_pcsCoords; }
		PendelumCoords GetSize() { return m_pcSize; }
		virtual void SetColor(int r, int g, int b, int a) { m_nRed = r; m_nGreen = g; m_nBlue = b; m_nAlpha = a; }
		void Scale(float& flPercentage) { 
			PendelumCoords pcOldSize = m_pcSize;
			m_pcSize *= flPercentage; 
			PendelumCoords pcDifference = pcOldSize - m_pcSize;
			pcDifference /= 2.0f;
			m_pcsCoords.m_Min -= pcDifference;
			m_pcsCoords.m_Max += pcDifference;
		}

		PendelumCoords ClipCoordsToObject(PendelumCoords coords)
		{			
			coords.x = min(max(m_pcsCoords.m_Min.x, coords.x), m_pcsCoords.m_Max.x);
			coords.y = min(max(m_pcsCoords.m_Min.y, coords.y), m_pcsCoords.m_Max.y);
			return coords;
		}

		bool OutSideObjectBounds(PendelumCoords coord)
		{
			if ((coord.x > m_pcsCoords.m_Min.x) && (coord.y > m_pcsCoords.m_Min.y) && (coord.y < m_pcsCoords.m_Max.y) && (m_pcsCoords.m_Max.x > coord.x))
				return true;

			return false;
		}
		
		bool ClipCoordSet(PendelumCoordSet& ClipCoords)
		{
			// TODO : Add Support for the Other Two points!
			//PendelumCoordSet OtherTwo = { {ClipCoords.m_Min.x, ClipCoords.m_Max.y}, {ClipCoords.m_Max.x, ClipCoords.m_Min.y} };
			if (OutSideObjectBounds(ClipCoords.m_Min) && OutSideObjectBounds(ClipCoords.m_Max))
				return false;
			else
			{
				ClipCoords.m_Min = ClipCoordsToObject(ClipCoords.m_Min);
				ClipCoords.m_Max = ClipCoordsToObject(ClipCoords.m_Max);
			}
			return true;
		}




	protected:
		PendelumCoordSet m_pcsCoords;
		PendelumCoords m_pcSize;
		int m_nRed, m_nGreen, m_nBlue, m_nAlpha = 255;
	private:
		const char* m_szDebugName = "";
	};



	class Pendelum3Container
	{
	public:
		
		virtual PendelumCoordSet ContainerGetCoords() = 0; // I ain't smart enough to figure out how to not have ambigous access lol
		PendelumCoords GetDefaultMemberPadding()
		{
			return m_pcMemberPadding;
		}


		void SetPadding(float x, float y)
		{
			m_pcMemberPadding = { x,y };
		}

	protected:
		std::vector<Pendelum3MenuItem*> m_vMenuItems;
		PendelumCoords m_pcMemberPadding;
		bool m_bTitleBar = false;
		PendelumCoords m_pcCursorPos = { 0,0 };
	};

	class Pendelum3Window :  public Pendelum3Container, public Pendelum3Object
	{
	public:
		Pendelum3Window(){}
		Pendelum3Window(PendelumCoordSet pcsCoords, char* sWindowTitle)
		{
			m_pcsCoords = pcsCoords;
			RecalculateSize();
			strcpy_s(m_sWindowName, sizeof(m_sWindowName), sWindowTitle);
		}
		Pendelum3Window(float x, float y, float x2, float y2, char* sWindowTitle)
		{
			m_pcsCoords = { x,y,x2,y2 };
			RecalculateSize();
			strcpy_s(m_sWindowName, sizeof(m_sWindowName), sWindowTitle);
		}
		Pendelum3Window(float x, float y, char* sWindowTitle)
		{
			m_pcsCoords.m_Min = { x,y };
			strcpy_s(m_sWindowName, sizeof(m_sWindowName), sWindowTitle);
		}
		Pendelum3Window(float x, float y, PendelumCoords pcSize, char* sWindowTitle)
		{
			m_pcsCoords.m_Min = { x,y };
			m_pcSize = pcSize;
			RecalculateCoordinates();
			strcpy_s(m_sWindowName, sizeof(m_sWindowName), sWindowTitle);
		}

		virtual PendelumCoordSet ContainerGetCoords()
		{
			return m_pcsCoords;
		}

		void AddMember(Pendelum3MenuItem* pItem);



		virtual void Render() = 0;
	protected:
		char m_sWindowName[MAX_PATH];

	};


	class Pendelum3MenuItem : public Pendelum3Object
	{
	public:
		__forceinline void UpdateSize()
		{
			RecalculateSize();
		};
		void __declspec(noinline) UpdateCoords()
		{
			if(m_pContainer)
				m_pcsCoords.m_Min = m_pContainer->ContainerGetCoords().m_Min + m_pcsRelativeCoords.m_Min + m_pContainer->GetDefaultMemberPadding();
			RecalculateCoordinates();
		};


		void __declspec(noinline) SetRelativeCoordinates(float x, float y) 
		{
			m_pcsRelativeCoords.m_Min = { x,y };
			UpdateCoords();
		}

		__forceinline void SetContainer(Pendelum3Container* pMaster)
		{
			m_pContainer = pMaster;
		}

	protected:
		bool m_bIsRelative = true;
		Pendelum3Container* m_pContainer;
		PendelumCoordSet m_pcsRelativeCoords;
	};


	template <class T>
	class Pendelum3ValueItem : public Pendelum3MenuItem
	{
	public:
		Pendelum3ValueItem() {}
		Pendelum3ValueItem(T* val) { m_pValue = val; }
		virtual void Render() = 0;
		void SetValuePtr(T* pValue)
		{
			m_pValue = pValue;
		}

		void SetValue(T val)
		{
			*m_pValue = val;
		}

		void ClampVal()
		{
			if (*m_pValue > m_ValMax)
				*m_pValue = m_ValMax;
			else if (*m_pValue < m_ValMin)
				*m_pValue = m_ValMin;
		}

		void SetValueMin(T min)
		{
			m_ValMin = min;
		}

		void SetValueMax(T max)
		{
			m_ValMax = max;
		}



	protected:
		char m_szText[MAX_PATH] = { "\0" };
		char m_szHelpString[MAX_PATH] = { "\0" };
		T* m_pValue = nullptr;
		T m_ValMin;
		T m_ValMax;
	};

	class Pendelum3SubMenu : public Pendelum3MenuItem, public Pendelum3Container
	{
	public:

		PendelumCoords GetDefaultMemberPadding()
		{
			return m_pcMemberPadding;
		}


		void AddMember(Pendelum3MenuItem* pItem)
		{
			m_vMenuItems.push_back(pItem);
			pItem->SetContainer(this);
		}

		virtual PendelumCoordSet ContainerGetCoords()
		{
			return m_pcsCoords;
		}



	protected:
		Pendelum3Window* m_pMasterWindow;
		std::vector<Pendelum3MenuItem*> m_vMenuItems;
		bool m_bTitleBar = false;
		char m_sWindowName[MAX_PATH];
	};





	namespace Pend3Renderer
	{

	}
}



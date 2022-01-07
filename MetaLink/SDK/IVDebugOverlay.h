#pragma once
#include "mathlib/mathlib.h"


class  OverlayText_t
{
public:
	OverlayText_t()
	{
		m_pNextOverlayText = 0;
		m_vOrigin = { 0,0,0 };
		m_bUseOrigin = false;
		m_nLineOffset = 0;
		m_flXPos = 0;
		m_flYPos = 0;
		m_szText[0] = 0;
		m_flEndTime = 0.0f;
		m_nServerCount = -1;
		m_nCreationTick = -1;
		r = g = b = a = 255;
	}

	bool			IsDead();
	void			SetEndTime(float duration) { m_flEndTime = duration; }

	Vector			m_vOrigin;
	bool			m_bUseOrigin;
	int				m_nLineOffset;
	float			m_flXPos;
	float			m_flYPos;
	char			m_szText[512];
	float			m_flEndTime;			// When does this text go away
	int				m_nCreationTick;		// If > 0, show only one server frame
	int				m_nServerCount;			// compare server spawn count to remove stale overlays
	int				r;
	int				g;
	int				b;
	int				a;
	OverlayText_t* m_pNextOverlayText;
};

class IVDebugOverlay
    {
    public:
        virtual void            AddEntityTextOverlay(int ent_index, int line_offset, float duration, int r, int g, int b, int a, const char *format, ...) = 0;
        virtual void            AddBoxOverlay(const Vector& origin, const Vector& mins, const Vector& max, QAngle const& orientation, int r, int g, int b, int a, float duration) = 0;
        virtual void            AddSphereOverlay(const Vector& vOrigin, float flRadius, int nTheta, int nPhi, int r, int g, int b, int a, float flDuration) = 0;
        virtual void            AddTriangleOverlay(const Vector& p1, const Vector& p2, const Vector& p3, int r, int g, int b, int a, bool noDepthTest, float duration) = 0;
		virtual void            __unkn() = 0;
		virtual void            AddLineOverlay(const Vector& origin, const Vector& dest, int r, int g, int b, bool noDepthTest, float duration) = 0;
        virtual void            AddTextOverlay(const Vector& origin, float duration, const char *format, ...) = 0;
        virtual void            AddTextOverlay(const Vector& origin, int line_offset, float duration, const char *format, ...) = 0;
        virtual void            AddScreenTextOverlay(float flXPos, float flYPos, float flDuration, int r, int g, int b, int a, const char *text) = 0;
        virtual void            AddSweptBoxOverlay(const Vector& start, const Vector& end, const Vector& mins, const Vector& max, const QAngle& angles, int r, int g, int b, int a, float flDuration) = 0;
        virtual void            AddGridOverlay(const Vector& origin) = 0;
        virtual void            AddCoordFrameOverlay(const matrix3x4_t& frame, float flScale, int vColorTable[3][3] = NULL) = 0;
        virtual int             ScreenPosition(const Vector& point, Vector& screen) = 0;
        virtual int             ScreenPosition(float flXPos, float flYPos, Vector& screen) = 0;
        virtual OverlayText_t*  GetFirst(void) = 0;
        virtual OverlayText_t*  GetNext(OverlayText_t *current) = 0;
        virtual void            ClearDeadOverlays(void) = 0;
        virtual void            ClearAllOverlays() = 0;
        virtual void            AddTextOverlayRGB(const Vector& origin, int line_offset, float duration, float r, float g, float b, float alpha, const char *format, ...) = 0;
        virtual void            AddTextOverlayRGB(const Vector& origin, int line_offset, float duration, int r, int g, int b, int a, const char *format, ...) = 0;
        virtual void            AddLineOverlayAlpha(const Vector& origin, const Vector& dest, int r, int g, int b, int a, bool noDepthTest, float duration) = 0;
        virtual void            AddBoxOverlay2(const Vector& origin, const Vector& mins, const Vector& max, QAngle const& orientation, const uint8_t* faceColor, const uint8_t* edgeColor, float duration) = 0;
        virtual void            PurgeTextOverlays() = 0;
		virtual void            AddCapsuleOverlay(const Vector& mins, const Vector& max, float& diameter, int r, int g, int b, int a, float duration, int unknown, int occlude) = 0;
        //virtual void            AddCapsuleOverlay(const Vector& mins, const Vector& max, float& diameter, int r, int g, int b, int a, float duration) = 0;
		virtual void            AddCapsuleOverlay2(const Vector& mins, const Vector& max, float& diameter, int r, int g, int b, int a, float duration) = 0;
     
    private:
        inline void AddTextOverlay(const Vector& origin, int line_offset, float duration, int r, int g, int b, int a, const char *format, ...) {} /* catch improper use of bad interface. Needed because '0' duration can be resolved by compiler to NULL format string (i.e., compiles but calls wrong function) */
};
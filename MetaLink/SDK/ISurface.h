#pragma once

#include "mathlib/mathlib.h"
#include "mathlib/vector2d.h"
#include "VPanel.h"
#include "ImageFormat.h"
#include "IAppSystem.h"
#include "VirtualFunction.h"
#include <math.h>
#ifdef CreateFont
#undef CreateFont
#endif

#ifdef PlaySound
#undef PlaySound
#endif

class Color;
class IVguiMatInfo;

constexpr auto DegreesToRadians = [](float degrees) constexpr noexcept { return degrees * static_cast<float>(M_PI) / 180.0f; };
constexpr auto RadiansToDegrees = [](float radians) constexpr noexcept { return radians * 180.0f / static_cast<float>(M_PI); };

namespace vgui
{

	class Image;
	class Point;
	class IHTML;

	class IImage;
	class IHTMLEvents;
	// handles
	typedef unsigned long HCursor;
	typedef unsigned long HTexture;
	typedef unsigned long HFont;
}


struct Vertex_t
{
	Vertex_t() {}
	Vertex_t(const Vector2D& pos, const Vector2D& coord = Vector2D(0, 0))
	{
		m_Position = pos;
		m_TexCoord = coord;
	}
	void Init(const Vector2D& pos, const Vector2D& coord = Vector2D(0, 0))
	{
		m_Position = pos;
		m_TexCoord = coord;
	}

	Vector2D	m_Position;
	Vector2D	m_TexCoord;
};


enum FontDrawType_t
{
	// Use the "additive" value from the scheme file
	FONT_DRAW_DEFAULT = 0,

	// Overrides
	FONT_DRAW_NONADDITIVE,
	FONT_DRAW_ADDITIVE,

	FONT_DRAW_TYPE_COUNT = 2,
};


// Refactor these two
struct CharRenderInfo
{
	// In:
	FontDrawType_t	drawType;
	wchar_t			ch;

	// Out
	bool			valid;

	// In/Out (true by default)
	bool			shouldclip;
	// Text pos
	int				x, y;
	// Top left and bottom right
	Vertex_t		verts[2];
	int				textureId;
	int				abcA;
	int				abcB;
	int				abcC;
	int				fontTall;
	vgui::HFont		currentFont;
};

struct IntRect
{
	int x0;
	int y0;
	int x1;
	int y1;
};




class ISurface /*: public IAppSystem*/
	{
	public:
		enum SurfaceFeature_e
		{		// returns true if the surface supports minimize & maximize capabilities
			ANTIALIASED_FONTS = 1,
			DROPSHADOW_FONTS = 2,
			ESCAPE_KEY = 3,
			OPENING_NEW_HTML_WINDOWS = 4,
			FRAME_MINIMIZE_MAXIMIZE = 5,
			OUTLINE_FONTS = 6,
			DIRECT_HWND_RENDER = 7,
		};
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	virtual void DrawSetColor(Color col) = 0;
	virtual void DrawFilledRect(int x0, int y0, int x1, int y1) = 0;
	virtual void DrawFilledRectArray( IntRect *pRects, int numRects ) = 0;
	virtual void DrawOutlinedRect(int x0, int y0, int x1, int y1) = 0;
	virtual void DrawLine(int x0, int y0, int x1, int y1) = 0;
	virtual void DrawPolyLine(int *px, int *py, int numPoints) = 0;
	SUBFUNC();
	SUBFUNC();
	virtual void DrawSetTextFont(vgui::HFont font) = 0;
	virtual void DrawSetTextColor(int r, int g, int b, int a) = 0;
	virtual void DrawSetTextColor(Color col) = 0;
	virtual void DrawSetTextPos(int x, int y) = 0;
	virtual void DrawGetTextPos(int& x,int& y) = 0;
	virtual void DrawPrintText(const wchar_t *text, int textLen, FontDrawType_t drawType = FONT_DRAW_DEFAULT ) = 0;
	virtual void DrawUnicodeChar(wchar_t wch, FontDrawType_t drawType = FONT_DRAW_DEFAULT ) = 0;
	virtual void DrawFlushText() = 0;		// flushes any buffered text (for rendering optimizations)
	virtual vgui::IHTML *CreateHTMLWindow(vgui::IHTMLEvents *events,vgui::VPANEL context)=0;
	virtual void PaintHTMLWindow(vgui::IHTML *htmlwin) =0;
	virtual void DeleteHTMLWindow(vgui::IHTML *htmlwin)=0;
	virtual int	 DrawGetTextureId( char const *filename ) = 0;
	virtual bool DrawGetTextureFile(int id, char *filename, int maxlen ) = 0;
	virtual void DrawSetTextureFile(int id, const char *filename, int hardwareFilter, bool forceReload) = 0;
	virtual void DrawSetTextureRGBA(int id, const unsigned char *rgba, int wide, int tall, int hardwareFilter, bool forceReload)=0;
	virtual void DrawSetTexture(int id) = 0;
	virtual void DrawGetTextureSize(int id, int &wide, int &tall) = 0;
	virtual void DrawTexturedRect(int x0, int y0, int x1, int y1) = 0;
	virtual bool IsTextureIDValid(int id) = 0;
	SUBFUNC();
	virtual int CreateNewTextureID( bool procedural = false ) = 0;
	virtual void GetScreenSize(int &wide, int &tall) = 0;
	virtual void SetAsTopMost(vgui::VPANEL panel, bool state) = 0;
	virtual void BringToFront(vgui::VPANEL panel) = 0;
	virtual void SetForegroundWindow (vgui::VPANEL panel) = 0;
	virtual void SetPanelVisible(vgui::VPANEL panel, bool state) = 0;
	virtual void SetMinimized(vgui::VPANEL panel, bool state) = 0;
	virtual bool IsMinimized(vgui::VPANEL panel) = 0;
	virtual void FlashWindow(vgui::VPANEL panel, bool state) = 0;
	virtual void SetTitle(vgui::VPANEL panel, const wchar_t *title) = 0;
	virtual void SetAsToolBar(vgui::VPANEL panel, bool state) = 0;		// removes the window's task bar entry (for context menu's, etc.)
	virtual void CreatePopup(vgui::VPANEL panel, bool minimised, bool showTaskbarIcon = true, bool disabled = false, bool mouseInput = true , bool kbInput = true) = 0;
	virtual void SwapBuffers(vgui::VPANEL panel) = 0;
	virtual void Invalidate(vgui::VPANEL panel) = 0;
	virtual void SetCursor(vgui::HCursor cursor) = 0;
	virtual bool IsCursorVisible() = 0;
	virtual void ApplyChanges() = 0;
	virtual bool IsWithin(int x, int y) = 0;
	virtual bool HasFocus() = 0;
	virtual bool SupportsFeature(SurfaceFeature_e feature) = 0;
	virtual void RestrictPaintToSinglePanel(vgui::VPANEL panel, bool bForceAllowNonModalSurface = false) = 0;
	virtual void SetModalPanel(vgui::VPANEL ) = 0;
	virtual vgui::VPANEL GetModalPanel() = 0;
	virtual void UnlockCursor() = 0;
	virtual void LockCursor() = 0;
	virtual void SetTranslateExtendedKeys(bool state) = 0;
	virtual vgui::VPANEL GetTopmostPopup() = 0;
	virtual void SetTopLevelFocus(vgui::VPANEL panel) = 0;

	// fonts
	// creates an empty handle to a vgui font.  windows fonts can be add to this via SetFontGlyphSet().
	virtual vgui::HFont CreateFont() = 0;

	// adds to the font
	enum EFontFlags
	{
		FONTFLAG_NONE,
		FONTFLAG_ITALIC			= 0x001,
		FONTFLAG_UNDERLINE		= 0x002,
		FONTFLAG_STRIKEOUT		= 0x004,
		FONTFLAG_SYMBOL			= 0x008,
		FONTFLAG_ANTIALIAS		= 0x010,
		FONTFLAG_GAUSSIANBLUR	= 0x020,
		FONTFLAG_ROTARY			= 0x040,
		FONTFLAG_DROPSHADOW		= 0x080,
		FONTFLAG_ADDITIVE		= 0x100,
		FONTFLAG_OUTLINE		= 0x200,
		FONTFLAG_CUSTOM			= 0x400,		// custom generated font - never fall back to asian compatibility mode
		FONTFLAG_BITMAP			= 0x800,		// compiled bitmap font - no fallbacks
	};

	virtual bool SetFontGlyphSet(vgui::HFont font, const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags) = 0;

	// adds a custom font file (only supports true type font files (.ttf) for now)
	virtual bool AddCustomFontFile(const char *fontFileName) = 0;

	// returns the details about the font
	virtual int GetFontTall(vgui::HFont font) = 0;
	virtual int GetFontAscent(vgui::HFont font, wchar_t wch) = 0;
	virtual bool IsFontAdditive(vgui::HFont font) = 0;
	virtual void GetCharABCwide(vgui::HFont font, int ch, int &a, int &b, int &c) = 0;
	virtual int GetCharacterWidth(vgui::HFont font, int ch) = 0;
	virtual void GetTextSize(vgui::HFont font, const wchar_t *text, int &wide, int &tall) = 0;

	// notify icons?!?
	virtual vgui::VPANEL GetNotifyPanel() = 0;
	virtual void SetNotifyIcon(vgui::VPANEL context, vgui::HTexture icon, vgui::VPANEL panelToReceiveMessages, const char *text) = 0;

	// plays a sound
	virtual void PlaySound(const char *fileName) = 0;

	//!! these functions should not be accessed directly, but only through other vgui items
	//!! need to move these to seperate interface
	virtual int GetPopupCount() = 0;
	virtual vgui::VPANEL GetPopup(int index) = 0;
	virtual bool ShouldPaintChildPanel(vgui::VPANEL childPanel) = 0;
	virtual bool RecreateContext(vgui::VPANEL panel) = 0;
	virtual void AddPanel(vgui::VPANEL panel) = 0;
	virtual void ReleasePanel(vgui::VPANEL panel) = 0;
	virtual void MovePopupToFront(vgui::VPANEL panel) = 0;
	virtual void MovePopupToBack(vgui::VPANEL panel) = 0;

	virtual void SolveTraverse(vgui::VPANEL panel, bool forceApplySchemeSettings = false) = 0;
	virtual void PaintTraverse(vgui::VPANEL panel) = 0;

	virtual void EnableMouseCapture(vgui::VPANEL panel, bool state) = 0;

	// returns the size of the workspace
	virtual void GetWorkspaceBounds(int &x, int &y, int &wide, int &tall) = 0;

	// gets the absolute coordinates of the screen (in windows space)
	virtual void GetAbsoluteWindowBounds(int &x, int &y, int &wide, int &tall) = 0;

	// gets the base resolution used in proportional mode
	virtual void GetProportionalBase( int &width, int &height ) = 0;

	virtual void CalculateMouseVisible() = 0;
	virtual bool NeedKBInput() = 0;

	virtual bool HasCursorPosFunctions() = 0;
	virtual void SurfaceGetCursorPos(int &x, int &y) = 0;
	virtual void SurfaceSetCursorPos(int x, int y) = 0;


	// SRC only functions!!!
	virtual void DrawTexturedLine( const Vertex_t &a, const Vertex_t &b ) = 0;
	virtual void DrawOutlinedCircle(int x, int y, int radius, int segments) = 0;
	virtual void DrawTexturedPolyLine( const Vertex_t *p,int n ) = 0; // (Note: this connects the first and last points).
	virtual void DrawTexturedSubRect( int x0, int y0, int x1, int y1, float texs0, float text0, float texs1, float text1 ) = 0;
	virtual void DrawTexturedPolygon(int n, Vertex_t *pVertices) = 0;
	virtual const wchar_t *GetTitle(vgui::VPANEL panel) = 0;
	virtual bool IsCursorLocked( void ) const = 0;
	virtual void SetWorkspaceInsets( int left, int top, int right, int bottom ) = 0;

	// Lower level char drawing code, call DrawGet then pass in info to DrawRender
	virtual bool DrawGetUnicodeCharRenderInfo( wchar_t ch, CharRenderInfo& info ) = 0;
	virtual void DrawRenderCharFromInfo( const CharRenderInfo& info ) = 0;

	// global alpha setting functions
	// affect all subsequent draw calls - shouldn't normally be used directly, only in Panel::PaintTraverse()
	virtual void DrawSetAlphaMultiplier( float alpha /* [0..1] */ ) = 0;
	virtual float DrawGetAlphaMultiplier() = 0;

	// web browser
	virtual void SetAllowHTMLJavaScript( bool state ) = 0;

	// video mode changing
	virtual void OnScreenSizeChanged( int nOldWidth, int nOldHeight ) = 0;
#if !defined( _XBOX )
	virtual vgui::HCursor	CreateCursorFromFile( char const *curOrAniFile, char const *pPathID = 0 ) = 0;
#endif
	// create IVguiMatInfo object ( IMaterial wrapper in VguiMatSurface, NULL in CWin32Surface )
	virtual IVguiMatInfo *DrawGetTextureMatInfoFactory( int id ) = 0;

	virtual void PaintTraverseEx(vgui::VPANEL panel, bool paintPopups = false ) = 0;

	virtual float GetZPos() const = 0;

	// From the Xbox
	virtual void SetPanelForInput( vgui::VPANEL vpanel ) = 0;
	virtual void DrawFilledRectFade( int x0, int y0, int x1, int y1, unsigned int alpha0, unsigned int alpha1, bool bHorizontal ) = 0;
	virtual void DrawSetTextureRGBAEx(/*int id, const unsigned char *rgba, int wide, int tall, ImageFormat imageFormat*/ ) = 0;
	virtual void DrawSetTextScale(float sx, float sy) = 0;
	virtual bool SetBitmapFontGlyphSet(vgui::HFont font, const char *windowsFontName, float scalex, float scaley, int flags) = 0;
	// adds a bitmap font file
	virtual bool AddBitmapFontFile(const char *fontFileName) = 0;
	// sets a symbol for the bitmap font
	virtual void SetBitmapFontName( const char *pName, const char *pFontFilename ) = 0;
	// gets the bitmap font filename
	virtual const char *GetBitmapFontName( const char *pName ) = 0;

	virtual vgui::IImage *GetIconImageForFullPath( char const *pFullPath ) = 0;
	virtual void DrawUnicodeString( const wchar_t *pwString, FontDrawType_t drawType = FONT_DRAW_DEFAULT ) = 0;


	template <typename T>
	void DrawCircle(T x, T y, int startRadius, int radius) noexcept
	{
		int xs[12];
		int ys[12];

		for (int i = startRadius; i <= radius; ++i) {
			for (int j = 0; j < 12; ++j) {
				xs[j] = static_cast<int>(std::cos(DegreesToRadians(static_cast<float>(j * 30))) * i + x);
				ys[j] = static_cast<int>(std::sin(DegreesToRadians(static_cast<float>(j * 30))) * i + y);
			}
			DrawPolyLine(xs, ys, 12);
		}
	}

	std::pair<int,int> GetTextSize(unsigned font, const wchar_t* text) noexcept
	{
		int width, height;	
		CallVirtualFunction<void, unsigned, const wchar_t*>(this, 79, font, text, std::ref(width), std::ref(height));
		return std::make_pair(width, height);
	}
	static constexpr unsigned font{ 0x1d }; // builtin font from vgui_spew_fonts
};

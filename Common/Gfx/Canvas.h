/* Copyright (C) 2013 Rainmeter Project Developers
 *
 * This Source Code Form is subject to the terms of the GNU General Public
 * License; either version 2 of the License, or (at your option) any later
 * version. If a copy of the GPL was not distributed with this file, You can
 * obtain one at <https://www.gnu.org/licenses/gpl-2.0.html>. */

#ifndef RM_GFX_CANVAS_H_
#define RM_GFX_CANVAS_H_

#include "FontCollection.h"
#include "TextFormat.h"

namespace Gfx {

enum class Renderer
{
	GDIP,
	D2D,

	// Attempts to use D2D. If D2D is not available, fallbacks to use GDI+.
	PreferD2D
};

// Provides methods for drawing text, bitmaps, etc.
class __declspec(novtable) Canvas
{
public:
	virtual ~Canvas();

	Canvas(const Canvas& other) = delete;

	// Creates the canvas using the specified rendering engine. May return nullptr.
	static Canvas* Create(Renderer renderer);

	int GetW() const { return m_W; }
	int GetH() const { return m_H; }

	void SetAccurateText(bool option) { m_AccurateText = option; }

	// Resize the draw area of the Canvas. This function must not be called if BeginDraw() has been
	// called and has not yet been matched by a correspoding call to EndDraw.
	virtual void Resize(int w, int h);

	// BeginDraw() must be matched by a corresponding call to EndDraw(). Drawing functions must be
	// be called only between BeginDraw() and EndDraw().
	virtual bool BeginDraw() = 0;
	virtual void EndDraw() = 0;

	// Allows the use of Gdiplus::Graphics to perform drawing. Must be called between BeginDraw()
	// and EndDraw(). BeginGdiplusGraphicsContext() must be matched by a corresponding call to
	// EndGdiplusGraphicsContext(). While in the Gdiplus context, non-const member functions of
	// this class must not be called. 
	virtual Gdiplus::Graphics& BeginGdiplusContext() = 0;
	virtual void EndGdiplusContext() = 0;

	// Returns a read-only DC. Must be called between BeginDraw() and EndDraw(). GetDC() must be
	// matched by a corresponding call to ReleaseDC(). While in the Gdiplus context, non-const
	// member functions of this class must not be called.
	virtual HDC GetDC() = 0;
	virtual void ReleaseDC(HDC dc) = 0;

	// The Create* functions allocate objects specific to this Canvas object.
	virtual FontCollection* CreateFontCollection() = 0;
	virtual TextFormat* CreateTextFormat() = 0;

	virtual bool IsTransparentPixel(int x, int y) = 0;

	virtual void SetTransform(const Gdiplus::Matrix& matrix) = 0;
	virtual void ResetTransform() = 0;
	virtual void RotateTransform(float angle, float x, float y, float dx, float dy) = 0;

	virtual void SetAntiAliasing(bool enable) = 0;
	virtual void SetTextAntiAliasing(bool enable) = 0;

	virtual void Clear(const Gdiplus::Color& color = Gdiplus::Color(0, 0, 0, 0)) = 0;

	virtual void DrawTextW(const WCHAR* str, UINT strLen, const TextFormat& format, Gdiplus::RectF& rect,
		const Gdiplus::SolidBrush& brush, bool applyInlineFormatting = false) = 0;
	virtual bool MeasureTextW(const WCHAR* str, UINT strLen, const TextFormat& format, Gdiplus::RectF& rect) = 0;
	virtual bool MeasureTextLinesW(const WCHAR* str, UINT strLen, const TextFormat& format, Gdiplus::RectF& rect, UINT& lines) = 0;

	virtual void DrawBitmap(Gdiplus::Bitmap* bitmap, const Gdiplus::Rect& dstRect, const Gdiplus::Rect& srcRect) = 0;
	virtual void DrawMaskedBitmap(Gdiplus::Bitmap* bitmap, Gdiplus::Bitmap* maskBitmap, const Gdiplus::Rect& dstRect,
		const Gdiplus::Rect& srcRect, const Gdiplus::Rect& srcRect2) = 0;

	virtual void FillRectangle(Gdiplus::Rect& rect, const Gdiplus::SolidBrush& brush) = 0;

protected:
	Canvas();

	int m_W;
	int m_H;

	// GDI+, by default, includes padding around the string and also has a larger character spacing
	// compared to DirectWrite. In order to minimize diffeences between the text renderers,
	// an option is provided to enable accurate (typographic) text rendering. If set to |true|,
	// it is expected that there is no padding around the text and that the output is similar to
	// the default DirectWrite output. Otherwise, the expected result should be similar to that of
	// non-typographic GDI+.
	bool m_AccurateText;
};

}  // namespace Gfx

#endif

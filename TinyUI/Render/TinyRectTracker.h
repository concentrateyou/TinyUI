#pragma once
#include "../Control/TinyControl.h"
#include "TinyGDI.h"

namespace TinyUI
{
	class TinyRectTracker
	{
	public:
		TinyRectTracker();
		TinyRectTracker(LPCRECT lpSrcRect, UINT nStyle = (dottedLine | solidLine));
		TinyRectTracker(INT x, INT y, INT cx, INT cy, UINT nStyle = (dottedLine | solidLine));
		enum StyleFlags
		{
			solidLine = 1, dottedLine = 2, hatchedBorder = 4,
			resizeInside = 8, resizeOutside = 16, hatchInside = 32,
		};
		enum TrackerHit
		{
			hitNothing = -1,
			hitTopLeft = 0, hitTopRight = 1, hitBottomRight = 2, hitBottomLeft = 3,
			hitTop = 4, hitRight = 5, hitBottom = 6, hitLeft = 7, hitMiddle = 8
		};
		UINT m_nStyle;
		TinyRectangle m_rect;
		TinySize m_sizeMin;
		INT m_nHandleSize;
		static void Release();
		void GetTrueRect(LPRECT lpTrueRect);
		virtual BOOL SetCursor(TinyControl* pWindow, UINT nHitTest);
		virtual void Draw(TinyDC* pDC);
		virtual BOOL Track(TinyControl* pWindow, const TinyPoint& point, BOOL bAllowInvert = FALSE, TinyControl* pWindowClip = NULL);
		virtual BOOL TrackRubberBand(TinyControl* pWindow, const TinyPoint& point, BOOL bAllowInvert = TRUE);
		virtual INT HitTest(const TinyPoint& point);
		INT NormalizeHit(INT nHandle);
		void DrawDragRect(TinyDC* pDC, LPCRECT lpRect, SIZE size, LPCRECT lpRectLast, SIZE sizeLast, HBRUSH hBrush = NULL, HBRUSH hBrushLast = NULL);
		virtual void DrawTrackerRect(LPCRECT lpRect, TinyControl* pWindowClip, TinyDC* pDC, TinyControl* pWindow);
		virtual void AdjustRect(INT nHandle, LPRECT lpRect);
		virtual void OnChangedRect(const TinyRectangle& rectOld);
		virtual UINT GetHandleMask();
		Event<void(void*, const TinyRectangle&)> ChangedRect;
	public:
		virtual ~TinyRectTracker();
	protected:
		BOOL m_bAllowInvert;
		TinyRectangle m_rectLast;
		TinySize m_sizeLast;
		BOOL m_bErase;
		BOOL m_bFinalErase;
		void GetHandleRect(INT nHandle, TinyRectangle* pHandleRect);
		void GetModifyPointers(INT nHandle, INT**ppx, INT**ppy, INT* px, INT*py);
		virtual int HitTestHandles(const TinyPoint& point);
		virtual int GetHandleSize(LPCRECT lpRect = NULL);
		virtual BOOL TrackHandle(INT nHandle, TinyControl* pWindow, const TinyPoint& point, TinyControl* pWindowClip);
		virtual void MoveHandle(LPCRECT lpNewRect, LPRECT lpOldRect);
		void Construct();
	};
}



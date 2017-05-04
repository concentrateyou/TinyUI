#pragma once
#include "../Control/TinyControl.h"
#include "TinyGDI.h"

namespace TinyUI
{
	class TinyRectTracker
	{
		enum TrackerHit
		{
			hitNothing = -1,
			hitTopLeft = 0, hitTopRight = 1, hitBottomRight = 2, hitBottomLeft = 3,
			hitTop = 4, hitRight = 5, hitBottom = 6, hitLeft = 7, hitMiddle = 8
		};
	public:
		TinyRectTracker();
		virtual ~TinyRectTracker();
		virtual INT GetHandleSize() const;
		virtual void OnChangedRect(const TinyRectangle& rectOld);
		virtual BOOL AllowTracker() const;
		void Draw(TinyDC* pDC) const;
		INT HitTest(const TinyPoint& point) const;
		BOOL Track(HWND hWND, const TinyPoint& point, BOOL bAllowInvert);
		BOOL TrackRubberBand(HWND hWND, const TinyPoint& point, BOOL bAllowInvert);
		UINT GetHandleMask() const;
		BOOL SetCursor(HWND hWND, UINT nHitTest) const;
		void GetHandleRect(INT nHandle, TinyRectangle* pHandleRect) const;
		void SetEmpty();
	private:
		void Construct();
		INT NormalizeHit(INT nHandle) const;
		INT HitTestHandles(const TinyPoint& point) const;
		void AdjustRect(INT nHandle, LPRECT);
		void GetModifyPointers(INT nHandle, INT**ppx, INT**ppy, INT* px, INT*py);
		BOOL TrackHandle(INT nHandle, HWND hWND, const TinyPoint& point);
		void GetTrueRect(LPRECT lpTrueRect) const;
		
	public:
		INT				m_handleSize;
		BOOL			m_bErase;
		BOOL			m_bFinalErase;
		BOOL			m_bAllowInvert;
		TinySize		m_sizeMin;
		TinyRectangle	m_trackerRect;
		TinyPen			m_pen;
		TinyBrush		m_brush;
	};
}



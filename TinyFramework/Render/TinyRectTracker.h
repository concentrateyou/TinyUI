#pragma once
#include "../Control/TinyControl.h"
#include "TinyGDI.h"

namespace TinyFramework
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
		virtual INT		GetHandleSize() const;
		virtual void	OnChangedRect(const TinyRectangle& rectOld);
	public:
		INT		HitTest(const TinyPoint& point) const;
		UINT	GetHandleMask() const;
		BOOL	Track(HWND hWND, const TinyPoint& point, BOOL bAllowInvert);
		BOOL	TrackRubberBand(HWND hWND, const TinyPoint& point, BOOL bAllowInvert);
		BOOL	SetCursor(HWND hWND, UINT nHitTest) const;
		void	Draw(TinyDC* pDC) const;
		void	GetHandleRect(INT nHandle, TinyRectangle* pHandleRect) const;
		void	GetTrackerRect(TinyRectangle* pRect) const;
		void	SetEmpty();
		void	SetTrackerRect(const TinyRectangle& rectangle);
	private:
		INT		NormalizeHit(INT nHandle) const;
		INT		HitTestHandles(const TinyPoint& point) const;
		BOOL	TrackHandle(INT nHandle, HWND hWND, const TinyPoint& point);
		void	Construct();
		void	AdjustRect(INT nHandle, LPRECT);
		void	GetModifyPointers(INT nHandle, INT**ppx, INT**ppy, INT* px, INT*py);
		void	GetTrueRect(LPRECT lpTrueRect) const;
	protected:
		INT				m_handleSize;
		BOOL			m_bErase;
		BOOL			m_bFinalErase;
		BOOL			m_bAllowInvert;
		TinyPen			m_pen;
		TinyBrush		m_brush;
		TinySize		m_sizeMin;
		TinyRectangle	m_trackerRect;
	};
}



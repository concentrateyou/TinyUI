#pragma once
#include "TinyVisual.h"
#include "../Control/TinyScrollBox.h"

namespace TinyUI
{
	namespace Windowless
	{
		typedef struct tagSCROLLBARCALC {
			RECT	rectangle;
			RECT	arrowRectangle[2];
			RECT	thumbRectangle;
			RECT	pageRectangle[2];
		} SCROLLBARCALC, *LPSCROLLBARCALC;
		typedef struct tagSCROLLBOXINFO
		{
			INT     iMin;
			INT     iMax;
			INT		iPage;
			INT     iPos;
			INT		iThumbOffset;
			INT		iHitTest;
			INT		iHitTestPress;
		}SCROLLBARINFO, *LPSCROLLBARINFO;
		class TinyVisualScrollBar : public TinyVisual
		{
			friend class TinyVisualFactory;
			DECLARE_DYNAMIC(TinyVisualScrollBar);
			DISALLOW_COPY_AND_ASSIGN(TinyVisualScrollBar);
		protected:
			TinyVisualScrollBar(TinyVisual* spvisParent, TinyVisualTree* vtree);
		public:
			virtual ~TinyVisualScrollBar();
			TinyString	RetrieveTag() const OVERRIDE;
			INT			GetScrollPos();
			void		SetScrollInfo(INT iMin, INT iMax, INT iPage, INT iPos);
		public:
			Event<void(INT, INT)> EVENT_PosChange;
		private:
			INT			ScrollHitTest(const TinyPoint& pt);
			void		ScrollCalculate(SCROLLBARCALC* ps, const TinySize& size);
			void		ScrollTrackThumb(const TinyPoint& pt, SCROLLBARCALC* ps);
			void		DrawScrollBar(TinyClipCanvas& canvas, const TinySize& size);
			void		DrawArrow(TinyClipCanvas& canvas, SCROLLBARCALC* ps);
			void		DrawThumb(TinyClipCanvas& canvas, SCROLLBARCALC* ps);
			void		DrawGroove(TinyClipCanvas& canvas, SCROLLBARCALC* ps);
		protected:
			BOOL		OnDraw(HDC hDC, const RECT& rcPaint) OVERRIDE;
			HRESULT		OnMouseLeave();
			HRESULT		OnMouseMove(const TinyPoint& pos, DWORD dwFlags) OVERRIDE;
			HRESULT		OnLButtonDown(const TinyPoint& pos, DWORD dwFlags) OVERRIDE;
			HRESULT		OnLButtonUp(const TinyPoint& pos, DWORD dwFlags) OVERRIDE;
		private:
			BOOL			m_bVer;
			SCROLLBARINFO	m_si;
			TinyImage		m_images[9];
		};
	}
}




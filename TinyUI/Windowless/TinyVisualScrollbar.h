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

		enum ScrollImageStyle
		{
			ARROW1NORMAL = 0,
			ARROW1HIGHLIGHT = 1,
			ARROW1DOWN = 2,
			ARROW2NORMAL = 3,
			ARROW2HIGHLIGHT = 4,
			ARROW2DOWN = 5,
			SCROLLBARGROOVE = 6,
			SCROLLBARNORMAL = 7,
			SCROLLBARHIGHLIGHT = 8
		};

		/// <summary>
		/// 水平滚动条
		/// </summary>
		class TinyVisualHScrollBar : public TinyVisual
		{
			friend class TinyVisualDocument;
			DECLARE_DYNAMIC(TinyVisualHScrollBar)
			DISALLOW_COPY_AND_ASSIGN(TinyVisualHScrollBar)
		protected:
			TinyVisualHScrollBar(TinyVisual* spvisParent, TinyVisualDocument* document);
		public:
			virtual ~TinyVisualHScrollBar();
			TinyString	RetrieveTag() const OVERRIDE;
			BOOL		SetProperty(const TinyString& name, const TinyString& value) OVERRIDE;
			void		SetSize(const TinySize& size) OVERRIDE;
			INT			GetScrollPos() const;
			INT			GetPage() const;
			INT			GetMin() const;
			INT			GetMax() const;
			void		SetScrollInfo(INT iMin, INT iMax, INT iPage, INT iPos);
			void		SetScrollPos(INT iPos);
			void		SetStyleImage(ScrollImageStyle type, LPCSTR pzFile);
		public:
			Event<void(BOOL, INT, INT, INT)> EVENT_PosChange;
		protected:
			BOOL		OnDraw(HDC hDC, const RECT& rcPaint) OVERRIDE;
			HRESULT		OnCreate();
			HRESULT		OnMouseLeave();
			HRESULT		OnMouseMove(const TinyPoint& pos, DWORD dwFlags) OVERRIDE;
			HRESULT		OnMouseWheel(const TinyPoint& pos, SHORT zDelta, DWORD dwFlags) OVERRIDE;
			HRESULT		OnLButtonDown(const TinyPoint& pos, DWORD dwFlags) OVERRIDE;
			HRESULT		OnLButtonUp(const TinyPoint& pos, DWORD dwFlags) OVERRIDE;
		private:
			INT			ScrollHitTest(const TinyPoint& pt);
			void		ScrollCalculate(SCROLLBARCALC* ps, const TinySize& size);
			void		ScrollTrackThumb(const TinyPoint& pt, SCROLLBARCALC* ps);
			void		DrawScrollBar(TinyClipCanvas& canvas, const TinySize& size);
			void		DrawArrow(TinyClipCanvas& canvas, SCROLLBARCALC* ps);
			void		DrawThumb(TinyClipCanvas& canvas, SCROLLBARCALC* ps);
			void		DrawGroove(TinyClipCanvas& canvas, SCROLLBARCALC* ps);
		private:
			SCROLLBARINFO	m_si;
			TinyImage*		m_images[SCROLLBARHIGHLIGHT + 1];
		};
		/// <summary>
		/// 垂直滚动条
		/// </summary>
		class TinyVisualVScrollBar : public TinyVisual
		{
			friend class TinyVisualDocument;
			DECLARE_DYNAMIC(TinyVisualVScrollBar)
			DISALLOW_COPY_AND_ASSIGN(TinyVisualVScrollBar)
		protected:
			TinyVisualVScrollBar(TinyVisual* spvisParent, TinyVisualDocument* document);
		public:
			virtual ~TinyVisualVScrollBar();
			TinyString	RetrieveTag() const OVERRIDE;
			BOOL		SetProperty(const TinyString& name, const TinyString& value) OVERRIDE;
			void		SetSize(const TinySize& size) OVERRIDE;
			INT			GetScrollPos() const;
			INT			GetPage() const;
			INT			GetMin() const;
			INT			GetMax() const;
			void		SetScrollInfo(INT iMin, INT iMax, INT iPage, INT iPos);
			void		SetScrollPos(INT iPos);
			void		SetStyleImage(ScrollImageStyle type, LPCSTR pzFile);
		public:
			Event<void(BOOL, INT, INT, INT)> EVENT_PosChange;
		protected:
			BOOL		OnDraw(HDC hDC, const RECT& rcPaint) OVERRIDE;
			HRESULT		OnCreate() OVERRIDE;
			HRESULT		OnMouseLeave();
			HRESULT		OnMouseMove(const TinyPoint& pos, DWORD dwFlags) OVERRIDE;
			HRESULT		OnMouseWheel(const TinyPoint& pos, SHORT zDelta, DWORD dwFlags) OVERRIDE;
			HRESULT		OnLButtonDown(const TinyPoint& pos, DWORD dwFlags) OVERRIDE;
			HRESULT		OnLButtonUp(const TinyPoint& pos, DWORD dwFlags) OVERRIDE;
		private:
			INT			ScrollHitTest(const TinyPoint& pt);
			void		ScrollCalculate(SCROLLBARCALC* ps, const TinySize& size);
			void		ScrollTrackThumb(const TinyPoint& pt, SCROLLBARCALC* ps);
			void		DrawScrollBar(TinyClipCanvas& canvas, const TinySize& size);
			void		DrawArrow(TinyClipCanvas& canvas, SCROLLBARCALC* ps);
			void		DrawThumb(TinyClipCanvas& canvas, SCROLLBARCALC* ps);
			void		DrawGroove(TinyClipCanvas& canvas, SCROLLBARCALC* ps);
		private:
			BOOL			m_bVertical;
			SCROLLBARINFO	m_si;
			TinyImage*		m_images[SCROLLBARHIGHLIGHT + 1];
		};
	}
}




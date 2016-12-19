#pragma once
#include "TinyVisual.h"

namespace TinyUI
{
	namespace Windowless
	{
		class TinyVisualHScrollBar;
		/// <summary>
		/// 水平布局
		/// </summary>
		class TinyVisualHLayout : public TinyVisual
		{
			friend class TinyVisualDocument;
			DECLARE_DYNAMIC(TinyVisualHLayout)
			DISALLOW_COPY_AND_ASSIGN(TinyVisualHLayout)
		protected:
			TinyVisualHLayout(TinyVisual* spvisParent, TinyVisualDocument* vtree);
		public:
			virtual ~TinyVisualHLayout();
			TinyString RetrieveTag() const OVERRIDE;
		public:
			void AutoScroll(BOOL bEnable);
			void SetAlignment(HorizontalAlignment horizontalAlignment, VerticalAlignment verticalAlignment);
		protected:
			BOOL	OnDraw(HDC hDC, const RECT& rcPaint) OVERRIDE;
			HRESULT	OnMouseEnter() OVERRIDE;
			HRESULT	OnMouseLeave() OVERRIDE;
			HRESULT	OnCreate() OVERRIDE;
			HRESULT OnDestory() OVERRIDE;
			virtual void OnPosChange(BOOL, INT, INT, INT);
		private:
			void AdjustLayout(TinyVisual* spvis, INT dx, INT dy);
		private:
			BOOL					m_bAuto;
			HorizontalAlignment		m_horizontalAlignment;
			VerticalAlignment		m_verticalAlignment;
			TinyVisualHScrollBar*	m_scrollbar;
			TinyScopedPtr<Delegate<void(BOOL, INT, INT, INT)>> m_onPosChange;
		};
	}
}




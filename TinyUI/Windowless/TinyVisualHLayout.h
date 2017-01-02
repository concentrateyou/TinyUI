#pragma once
#include "TinyVisual.h"

namespace TinyUI
{
	namespace Windowless
	{
		class TinyVisualHScrollBar;
		/// <summary>
		/// 水平布局(子控件位置属性无效)
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
			BOOL IsLayout() const OVERRIDE;
			void OnSizeChange(const TinySize&, const TinySize&) OVERRIDE;
		public:
			void AutoScroll(BOOL bAuto);
			void SetHorizontalAlignment(HorizontalAlignment horizontalAlignment);
			void SetVerticalAlignment(VerticalAlignment verticalAlignment);
		protected:
			BOOL	OnDraw(HDC hDC, const RECT& rcPaint) OVERRIDE;
			HRESULT	OnMouseEnter() OVERRIDE;
			HRESULT	OnMouseLeave() OVERRIDE;
			HRESULT	OnCreate() OVERRIDE;
			HRESULT OnDestory() OVERRIDE;
		private:
			BOOL					m_bAuto;
			HorizontalAlignment		m_horizontalAlignment;
			VerticalAlignment		m_verticalAlignment;
		};
	}
}




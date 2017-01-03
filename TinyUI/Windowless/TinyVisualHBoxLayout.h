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
		class TinyVisualHBoxLayout : public TinyVisual
		{
			friend class TinyVisualDocument;
			DECLARE_DYNAMIC(TinyVisualHBoxLayout)
			DISALLOW_COPY_AND_ASSIGN(TinyVisualHBoxLayout)
		protected:
			TinyVisualHBoxLayout(TinyVisual* spvisParent, TinyVisualDocument* vtree);
		public:
			virtual ~TinyVisualHBoxLayout();
			TinyString RetrieveTag() const OVERRIDE;
			BOOL IsLayout() const OVERRIDE;
			HRESULT SetProperty(const TinyString& name, const TinyString& value) OVERRIDE;
			void OnSizeChange(const TinySize&, const TinySize&) OVERRIDE;
		public:
			void AutoScroll(BOOL bAuto);
			void SetAlignment(Alignment alignment);
		protected:
			BOOL	OnDraw(HDC hDC, const RECT& rcPaint) OVERRIDE;
			HRESULT	OnMouseEnter() OVERRIDE;
			HRESULT	OnMouseLeave() OVERRIDE;
			HRESULT	OnCreate() OVERRIDE;
			HRESULT OnDestory() OVERRIDE;
		private:
			BOOL					m_bAuto;
			Alignment				m_alignment;
		};
	}
}




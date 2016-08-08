#pragma once
#include "TinyVisual.h"
#include "TinyVisualScrollbar.h"

namespace TinyUI
{
	namespace Windowless
	{
		/// <summary>
		/// ап╠М©ь╪Ч
		/// </summary>
		class TinyVisualImageView : public TinyVisual
		{
			friend class TinyVisualFactory;
			DECLARE_DYNAMIC(TinyVisualImageView);
			DISALLOW_COPY_AND_ASSIGN(TinyVisualImageView);
		protected:
			TinyVisualImageView(TinyVisual* spvisParent, TinyVisualDocument* vtree);
		public:
			virtual ~TinyVisualImageView();
			TinyString RetrieveTag() const OVERRIDE;
		public:
			void SetScrollInfo(INT iMin, INT iMax, INT iPage, INT iPos);
			INT GetOffertY() const;
		protected:
			BOOL	OnDraw(HDC hDC, const RECT& rcPaint) OVERRIDE;
			HRESULT	OnMouseEnter() OVERRIDE;
			HRESULT	OnMouseLeave() OVERRIDE;
			HRESULT	OnCreate() OVERRIDE;
			HRESULT OnDestory() OVERRIDE;
			virtual void OnPosChange(INT, INT);
		private:
			TinyVisualVScrollBar*	m_scrollbar;
			TinyScopedPtr<Delegate<void(INT, INT)>> m_onPosChange;
		private:
			TinyImage	m_image;
			INT			m_offsetY;
		};
	}
}




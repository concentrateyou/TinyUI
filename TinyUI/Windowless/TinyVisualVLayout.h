#pragma once
#include "TinyVisual.h"

namespace TinyUI
{
	namespace Windowless
	{
		class TinyVisualVScrollBar;
		/// <summary>
		/// ´¹Ö±²¼¾Ö
		/// </summary>
		class TinyVisualVLayout : public TinyVisual
		{
			friend class TinyVisualDocument;
			DECLARE_DYNAMIC(TinyVisualVLayout)
			DISALLOW_COPY_AND_ASSIGN(TinyVisualVLayout)
		protected:
			TinyVisualVLayout(TinyVisual* spvisParent, TinyVisualDocument* vtree);
		public:
			virtual ~TinyVisualVLayout();
			TinyString RetrieveTag() const OVERRIDE;
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
			virtual void OnPosChange(BOOL, INT, INT, INT);
		private:
			void AdjustLayout(TinyVisual* spvis, INT dx, INT dy);
		private:
			BOOL					m_bAuto;
			HorizontalAlignment		m_horizontalAlignment;
			VerticalAlignment		m_verticalAlignment;
			TinyVisualVScrollBar*	m_scrollbar;
			TinyScopedPtr<Delegate<void(BOOL, INT, INT, INT)>> m_onPosChange;
		};
	}
}




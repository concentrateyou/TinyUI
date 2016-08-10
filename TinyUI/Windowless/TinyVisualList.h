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
		class TinyVisualList : public TinyVisual
		{
			friend class TinyVisualDocument;
			DECLARE_DYNAMIC(TinyVisualList);
			DISALLOW_COPY_AND_ASSIGN(TinyVisualList);
		protected:
			TinyVisualList(TinyVisual* spvisParent, TinyVisualDocument* vtree);
		public:
			virtual ~TinyVisualList();
			TinyString RetrieveTag() const OVERRIDE;
		public:
			void SetScrollInfo(INT iMin, INT iMax, INT iPage, INT iPos);
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
			TinyVisualVScrollBar*	m_scrollbar;
			TinyScopedPtr<Delegate<void(BOOL, INT, INT, INT)>> m_onPosChange;
		};
	}
}




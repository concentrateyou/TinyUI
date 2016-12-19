#pragma once
#include "TinyVisual.h"
#include "TinyVisualScrollbar.h"

namespace TinyUI
{
	namespace Windowless
	{
		/// <summary>
		/// ˮƽ����
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
			TinyVisualHScrollBar*	m_scrollbar;
			TinyScopedPtr<Delegate<void(BOOL, INT, INT, INT)>> m_onPosChange;
		};
	}
}




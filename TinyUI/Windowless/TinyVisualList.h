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
			friend class TinyVisualFactory;
			DECLARE_DYNAMIC(TinyVisualList);
			DISALLOW_COPY_AND_ASSIGN(TinyVisualList);
		protected:
			TinyVisualList(TinyVisual* spvisParent, TinyVisualTree* vtree);
		public:
			virtual ~TinyVisualList();
			TinyString RetrieveTag() const OVERRIDE;
		protected:
			BOOL	OnDraw(HDC hDC, const RECT& rcPaint) OVERRIDE;
			HRESULT	OnCreate() OVERRIDE;
			HRESULT OnDestory() OVERRIDE;
			virtual void OnPosChange(INT, INT);
		private:
			TinyVisualScrollBar*	m_scrollbar;
			TinyScopedPtr<Delegate<void(INT, INT)>> m_onPosChange;
		};
	}
}




#pragma once
#include "TinyVisual.h"

namespace TinyUI
{
	namespace Windowless
	{
		/// <summary>
		/// ÏµÍ³°´Å¥
		/// </summary>
		class TinyVisualButton : public TinyVisual
		{
			friend class TinyVisualFactory;
			DECLARE_DYNAMIC(TinyVisualButton);
			DISALLOW_COPY_AND_ASSIGN(TinyVisualButton);
		protected:
			TinyVisualButton(TinyVisual* spvisParent, TinyVisualTree* vtree);
		public:
			virtual ~TinyVisualButton();
			TinyString RetrieveTag() const OVERRIDE;
		protected:
			BOOL OnDraw(HDC hDC, const RECT& rcPaint) OVERRIDE;
			HRESULT OnMouseLeave() OVERRIDE;
			HRESULT	OnLButtonDown(TinyPoint pos, DWORD dwFlags) OVERRIDE;
			HRESULT	OnMouseMove(TinyPoint pos, DWORD dwFlags) OVERRIDE;
			HRESULT	OnLButtonUp(TinyPoint pos, DWORD dwFlags) OVERRIDE;
		private:
			StyleImage	m_dwFlag;
		};
	}
}




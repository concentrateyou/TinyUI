#pragma once
#include "TinyVisualElement.h"
#include "TinyVisualHWND.h"

namespace TinyUI
{
	namespace Windowless
	{
		class TinyVisualTree;
		class TinyVisualWindow : public TinyVisualElement
		{
		public:
			TinyVisualWindow(TinyVisualTree* pT);
		public:
			LPCSTR RetrieveTag() OVERRIDE;
			HRESULT OnDraw(TinyDC& dc, TinyRectangle& drawRect) OVERRIDE;
			HRESULT OnMouseMove(POINT pos) OVERRIDE;
			HRESULT OnLButtonDown(POINT pos) OVERRIDE;
			HRESULT OnLButtonUp(POINT pos) OVERRIDE;
			HRESULT OnRButtonDown(POINT pos) OVERRIDE;
			HRESULT OnRButtonUp(POINT pos) OVERRIDE;
		private:
			TinyVisualTree*		m_pT;
		};
	}
}




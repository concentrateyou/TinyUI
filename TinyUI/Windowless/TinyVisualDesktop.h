#pragma once
#include "TinyVisual.h"

namespace TinyUI
{
	namespace Windowless
	{
		class TinyVisualDesktop : public TinyVisual
		{
		public:
			virtual LPCSTR RetrieveTag() OVERRIDE;
			virtual HRESULT OnDraw(TinyDC& dc, TinyRectangle& drawRect) OVERRIDE;
			virtual HRESULT OnMouseMove(POINT pos) OVERRIDE;
			virtual HRESULT OnLButtonDown(POINT pos) OVERRIDE;
			virtual HRESULT OnLButtonUp(POINT pos) OVERRIDE;
			virtual HRESULT OnRButtonDown(POINT pos) OVERRIDE;
			virtual HRESULT OnRButtonUp(POINT pos) OVERRIDE;
		};
	}
}




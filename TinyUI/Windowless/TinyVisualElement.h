#pragma once
#include "TinyVisual.h"

namespace TinyUI
{
	namespace Windowless
	{
		/// <summary>
		/// 可视化元素额外的一些属性和方法
		/// </summary>
		class TinyVisualElement : public TinyVisual
		{
		public:
			 BOOL ParseVisual(LPCSTR pzText) OVERRIDE;
			 LPCSTR RetrieveTag() OVERRIDE;
			 HRESULT OnDraw(TinyDC& dc, TinyRectangle& drawRect) OVERRIDE;
			 HRESULT OnMouseMove(POINT pos) OVERRIDE;
			 HRESULT OnLButtonDown(POINT pos) OVERRIDE;
			 HRESULT OnLButtonUp(POINT pos) OVERRIDE;
			 HRESULT OnRButtonDown(POINT pos) OVERRIDE;
			 HRESULT OnRButtonUp(POINT pos) OVERRIDE;
		};
	}
}




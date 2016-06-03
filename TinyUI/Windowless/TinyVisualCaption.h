#pragma once
#include "TinyVisual.h"

namespace TinyUI
{
	namespace Windowless
	{
		/// <summary>
		/// 系统按钮
		/// </summary>
		class TinySysButton : public TinyVisual
		{
		public:
			TinySysButton(TinyVisual* spvisParent);
			virtual ~TinySysButton();
			TinyString RetrieveTag() const OVERRIDE;
			HRESULT OnDraw(TinyDC& dc, TinyRectangle& drawRect) OVERRIDE;
			HRESULT OnMouseMove(POINT pos) OVERRIDE;
			HRESULT OnLButtonDown(POINT pos) OVERRIDE;
			HRESULT OnLButtonUp(POINT pos) OVERRIDE;
			HRESULT OnRButtonDown(POINT pos) OVERRIDE;
			HRESULT OnRButtonUp(POINT pos) OVERRIDE;
		private:
			TinyImage	m_images[3];
		};
		/// <summary>
		/// 标题栏容器控件HorizontalLayout
		/// </summary>
		class TinyVisualCaption : public TinyVisual
		{
		public:
			TinyVisualCaption(TinyVisual* spvisParent);
			virtual ~TinyVisualCaption();
			TinyString RetrieveTag() const OVERRIDE;
			void Resize() OVERRIDE;
			HRESULT OnDraw(TinyDC& dc, TinyRectangle& drawRect) OVERRIDE;
			HRESULT OnMouseMove(POINT pos) OVERRIDE;
			HRESULT OnLButtonDown(POINT pos) OVERRIDE;
			HRESULT OnLButtonUp(POINT pos) OVERRIDE;
			HRESULT OnRButtonDown(POINT pos) OVERRIDE;
			HRESULT OnRButtonUp(POINT pos) OVERRIDE;
		};
	}
}




#pragma once
#include "TinyVisual.h"

namespace TinyUI
{
	namespace Windowless
	{
		/// <summary>
		/// 系统按钮
		/// </summary>
		class TinyVisualSysButton : public TinyVisual
		{
			friend class TinyVisualFactory;
			DECLARE_DYNAMIC(TinyVisualSysButton);
			DISALLOW_COPY_AND_ASSIGN(TinyVisualSysButton);
		protected:
			TinyVisualSysButton(TinyVisual* spvisParent, TinyVisualTree* vtree);
		public:
			virtual ~TinyVisualSysButton();
			TinyString RetrieveTag() const OVERRIDE;
			HRESULT OnDraw(TinyCanvas& canvas, TinyRectangle& drawRect) OVERRIDE;
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
			friend class TinyVisualFactory;
			DECLARE_DYNAMIC(TinyVisualCaption);
			DISALLOW_COPY_AND_ASSIGN(TinyVisualCaption);
		protected:
			TinyVisualCaption(TinyVisual* spvisParent, TinyVisualTree* vtree);
		public:
			virtual ~TinyVisualCaption();
			TinyString RetrieveTag() const OVERRIDE;
			void Resize() OVERRIDE;
			HRESULT OnDraw(TinyCanvas& canvas, TinyRectangle& drawRect) OVERRIDE;
			HRESULT OnMouseMove(POINT pos) OVERRIDE;
			HRESULT OnLButtonDown(POINT pos) OVERRIDE;
			HRESULT OnLButtonUp(POINT pos) OVERRIDE;
			HRESULT OnRButtonDown(POINT pos) OVERRIDE;
			HRESULT OnRButtonUp(POINT pos) OVERRIDE;
		};
	}
}




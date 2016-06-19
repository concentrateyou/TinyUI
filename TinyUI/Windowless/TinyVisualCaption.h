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
			BOOL OnDraw(HDC hDC) OVERRIDE;
			HRESULT OnMouseMove(TinyPoint pos, DWORD dwFlags) OVERRIDE;
			HRESULT OnLButtonDown(TinyPoint pos, DWORD dwFlags) OVERRIDE;
			HRESULT OnLButtonUp(TinyPoint pos, DWORD dwFlags) OVERRIDE;
		public:
			DECLARE_EVENT0(Click, void);
		};
		/// <summary>
		/// 标题栏容器控件
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
		};
	}
}




#pragma once
#include "TinyVisual.h"

namespace TinyUI
{
	namespace Windowless
	{
		class TinyVisualWindow : public TinyVisual
		{
			friend class TinyVisualTree;
			DECLARE_DYNAMIC(TinyVisualWindow);
			DISALLOW_COPY_AND_ASSIGN(TinyVisualWindow);
		protected:
			TinyVisualWindow(TinyVisual* spvisParent, TinyVisualTree* vtree);
		public:
			virtual ~TinyVisualWindow();
		public:
			TinyString RetrieveTag() const OVERRIDE;
			HRESULT OnDraw(TinyCanvas& canvas, TinyRectangle& drawRect) OVERRIDE;
			HRESULT OnMouseMove(POINT pos) OVERRIDE;
			HRESULT OnLButtonDown(POINT pos) OVERRIDE;
			HRESULT OnLButtonUp(POINT pos) OVERRIDE;
			HRESULT OnRButtonDown(POINT pos) OVERRIDE;
			HRESULT OnRButtonUp(POINT pos) OVERRIDE;
		};
	}
}




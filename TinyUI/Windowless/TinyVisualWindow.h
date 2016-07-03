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
			BOOL OnDraw(HDC hDC, const RECT& rcPaint) OVERRIDE;
		};
	}
}




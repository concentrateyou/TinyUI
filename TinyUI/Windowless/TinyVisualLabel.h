#pragma once
#include "TinyVisual.h"

namespace TinyUI
{
	namespace Windowless
	{
		/// <summary>
		/// ±êÌâÀ¸ÈÝÆ÷¿Ø¼þ
		/// </summary>
		class TinyVisualLabel : public TinyVisual
		{
			friend class TinyVisualDocument;
			DECLARE_DYNCREATE(TinyVisualLabel)
			DISALLOW_COPY_AND_ASSIGN(TinyVisualLabel)
		protected:
			TinyVisualLabel();
			TinyVisualLabel(TinyVisual* spvisParent, TinyVisualDocument* vtree);
		public:
			virtual ~TinyVisualLabel();
			TinyString RetrieveTag() const OVERRIDE;
		protected:
			BOOL OnDraw(HDC hDC, const RECT& rcPaint) OVERRIDE;
		};
	}
}




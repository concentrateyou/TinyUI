#pragma once
#include "TinyVisual.h"

namespace TinyUI
{
	namespace Windowless
	{
		/// <summary>
		/// ¶¯»­¿Ø¼þ
		/// </summary>
		class TinyVisualAnimate : public TinyVisual
		{
			friend class TinyVisualDocument;
			DECLARE_DYNCREATE(TinyVisualAnimate)
			DISALLOW_COPY_AND_ASSIGN(TinyVisualAnimate)
		protected:
			TinyVisualAnimate();
			TinyVisualAnimate(TinyVisual* spvisParent, TinyVisualDocument* vtree);
		public:
			virtual ~TinyVisualAnimate();
			TinyString RetrieveTag() const OVERRIDE;
		protected:
			BOOL OnDraw(HDC hDC, const RECT& rcPaint) OVERRIDE;
		};
	}
}




#pragma once
#include "TinyVisual.h"

namespace TinyUI
{
	namespace Windowless
	{
		/// <summary>
		/// Tab�ؼ�
		/// </summary>
		class TinyVisualTab : public TinyVisual
		{
			friend class TinyVisualDocument;
			DECLARE_DYNAMIC(TinyVisualTab)
			DISALLOW_COPY_AND_ASSIGN(TinyVisualTab)
		protected:
			TinyVisualTab(TinyVisual* spvisParent, TinyVisualDocument* vtree);
		public:
			virtual ~TinyVisualTab();
			TinyString RetrieveTag() const OVERRIDE;
		protected:
			BOOL OnDraw(HDC hDC, const RECT& rcPaint) OVERRIDE;
		};
	}
}




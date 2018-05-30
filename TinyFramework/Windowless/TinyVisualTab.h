#pragma once
#include "TinyVisual.h"

namespace TinyFramework
{
	namespace Windowless
	{
		/// <summary>
		/// Tabҳ
		/// </summary>
		class TinyVisualTabPage : public TinyVisual
		{

		};
		/// <summary>
		/// Tab�ؼ�
		/// </summary>
		class TinyVisualTab : public TinyVisual
		{
			friend class TinyVisualDocument;
			DECLARE_DYNCREATE(TinyVisualTab)
			DISALLOW_COPY_AND_ASSIGN(TinyVisualTab)
		protected:
			TinyVisualTab();
			TinyVisualTab(TinyVisual* spvisParent, TinyVisualDocument* vtree);
		public:
			virtual ~TinyVisualTab();
			TinyString RetrieveTag() const OVERRIDE;
		protected:
			BOOL OnDraw(HDC hDC, const RECT& rcPaint) OVERRIDE;
		};
	}
}




#pragma once
#include "TinyVisual.h"

namespace TinyFramework
{
	namespace Windowless
	{
		class TinyVisualComboBoxItem : public TinyVisual
		{
			friend class TinyVisualDocument;
			DECLARE_DYNCREATE(TinyVisualComboBoxItem)
			DISALLOW_COPY_AND_ASSIGN(TinyVisualComboBoxItem)
		protected:
			TinyVisualComboBoxItem();
			TinyVisualComboBoxItem(TinyVisual* spvisParent, TinyVisualDocument* document);
		public:
			virtual ~TinyVisualComboBoxItem();
			TinyString	RetrieveTag() const OVERRIDE;
			BOOL		OnDraw(HDC hDC, const RECT& rcPaint) OVERRIDE;
		};
	}
}




#pragma once
#include "TinyVisual.h"

namespace TinyFramework
{
	namespace Windowless
	{
		class TinyVisualLabel : public TinyVisual
		{
			friend class TinyVisualDocument;
			DECLARE_DYNCREATE(TinyVisualLabel)
			DISALLOW_COPY_AND_ASSIGN(TinyVisualLabel)
		protected:
			TinyVisualLabel();
			TinyVisualLabel(TinyVisual* spvisParent, TinyVisualDocument* document);
		public:
			virtual ~TinyVisualLabel();
			TinyString	RetrieveTag() const OVERRIDE;
		public:
			BOOL		OnDraw(HDC hDC, const RECT& rcPaint) OVERRIDE;
		};
	}
}




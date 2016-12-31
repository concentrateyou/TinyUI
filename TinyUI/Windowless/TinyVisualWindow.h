#pragma once
#include "TinyVisual.h"

namespace TinyUI
{
	namespace Windowless
	{
		class TinyVisualWindow : public TinyVisual
		{
			friend class TinyVisualBuilder;
			friend class TinyVisualDocument;
			DECLARE_DYNAMIC(TinyVisualWindow)
			DISALLOW_COPY_AND_ASSIGN(TinyVisualWindow)
		protected:
			TinyVisualWindow(TinyVisual* spvisParent, TinyVisualDocument* document);
		public:
			virtual ~TinyVisualWindow();

			void OnSizeChange(const TinySize&) OVERRIDE;

		public:
			TinyString RetrieveTag() const OVERRIDE;
			BOOL IsLayout() const OVERRIDE;
			BOOL OnDraw(HDC hDC, const RECT& rcPaint) OVERRIDE;
		};
	}
}




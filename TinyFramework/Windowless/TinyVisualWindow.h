#pragma once
#include "TinyVisual.h"

namespace TinyFramework
{
	namespace Windowless
	{
		class TinyVisualWindow : public TinyVisual
		{
			friend class TinyVisualDocument;
			DECLARE_DYNCREATE(TinyVisualWindow)
			DISALLOW_COPY_AND_ASSIGN(TinyVisualWindow)
		protected:
			TinyVisualWindow();
			TinyVisualWindow(TinyVisual* spvisParent, TinyVisualDocument* document);
		public:
			virtual ~TinyVisualWindow();
			TinyString	RetrieveTag() const OVERRIDE;
			void		SetPosition(const TinyPoint& pos) OVERRIDE;
			void		SetSize(const TinySize& size) OVERRIDE;
			void		SetText(const TinyString& pzText) OVERRIDE;
		public:
			BOOL		OnDraw(HDC hDC, const RECT& rcPaint) OVERRIDE;
		};
	}
}




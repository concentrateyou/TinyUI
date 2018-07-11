#pragma once
#include "TinyVisual.h"

namespace TinyFramework
{
	namespace Windowless
	{
		class TinyVisualNative : public TinyVisual
		{
			friend class TinyVisualDocument;
			DECLARE_DYNCREATE(TinyVisualNative)
			DISALLOW_COPY_AND_ASSIGN(TinyVisualNative)
		protected:
			TinyVisualNative();
			TinyVisualNative(TinyVisual* spvisParent, TinyVisualDocument* document);
		public:
			virtual ~TinyVisualNative();
			TinyString	RetrieveTag() const OVERRIDE;
			BOOL		OnDraw(HDC hDC, const RECT& rcPaint) OVERRIDE;
			void		SetPosition(const TinyPoint& pos) OVERRIDE;
			void		SetSize(const TinySize& size) OVERRIDE;
			void		SetVisible(BOOL visible) OVERRIDE;
			void		SetEnable(BOOL enable) OVERRIDE;
		private:
			HWND		m_hWND;
		};
	}
}




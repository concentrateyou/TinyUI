#pragma once
#include "TinyVisual.h"
#include "TinyTextHost.h"

namespace TinyUI
{
	namespace Windowless
	{
		class TinyVisualRichText : public TinyVisual
		{
			friend class TinyVisualFactory;
			DECLARE_DYNAMIC(TinyVisualRichText);
			DISALLOW_COPY_AND_ASSIGN(TinyVisualRichText);
		protected:
			TinyVisualRichText(TinyVisual* spvisParent, TinyVisualDocument* vtree);
		public:
			virtual ~TinyVisualRichText();
			TinyString RetrieveTag() const OVERRIDE;
		protected:
			HRESULT OnCreate() OVERRIDE;
			HRESULT OnDestory() OVERRIDE;
			BOOL	OnDraw(HDC hDC, const RECT& rcPaint) OVERRIDE;
		private:
			TinyTextHost	m_texthost;
		};
	}
}




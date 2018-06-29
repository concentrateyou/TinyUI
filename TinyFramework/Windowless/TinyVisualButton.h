#pragma once
#include "TinyVisual.h"

namespace TinyFramework
{
	namespace Windowless
	{
		class TinyVisualButton : public TinyVisual
		{
			friend class TinyVisualDocument;
			DECLARE_DYNCREATE(TinyVisualButton)
			DISALLOW_COPY_AND_ASSIGN(TinyVisualButton)
		protected:
			TinyVisualButton();
			TinyVisualButton(TinyVisual* spvisParent, TinyVisualDocument* document);
		public:
			virtual ~TinyVisualButton();
			TinyString	RetrieveTag() const OVERRIDE;
		};
	}
}




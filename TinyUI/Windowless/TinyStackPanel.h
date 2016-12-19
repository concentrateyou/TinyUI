#pragma once
#include "TinyVisual.h"

namespace TinyUI
{
	namespace Windowless
	{
		/// <summary>
		/// 垂直或者水平布局
		/// </summary>
		class TinyStackPanel : public TinyVisual
		{
			friend class TinyVisualDocument;
			DECLARE_DYNAMIC(TinyStackPanel)
			DISALLOW_COPY_AND_ASSIGN(TinyStackPanel)
		protected:
			TinyStackPanel(TinyVisual* spvisParent, TinyVisualDocument* vtree);
		public:
			virtual ~TinyStackPanel();
			TinyString RetrieveTag() const OVERRIDE;
		};
	}
}




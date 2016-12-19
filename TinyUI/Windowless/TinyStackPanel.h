#pragma once
#include "TinyVisual.h"

namespace TinyUI
{
	namespace Windowless
	{
		/// <summary>
		/// ��ֱ����ˮƽ����
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




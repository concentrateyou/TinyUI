#pragma once
#include "TinyVisual.h"

namespace TinyUI
{
	namespace Windowless
	{
		/// <summary>
		/// �����������ؼ�
		/// </summary>
		class TinyVisualCaption : public TinyVisual
		{
			friend class TinyVisualDocument;
			DECLARE_DYNAMIC(TinyVisualCaption)
			DISALLOW_COPY_AND_ASSIGN(TinyVisualCaption)
		protected:
			TinyVisualCaption(TinyVisual* spvisParent, TinyVisualDocument* vtree);
		public:
			virtual ~TinyVisualCaption();
			TinyString RetrieveTag() const OVERRIDE;
		};
	}
}




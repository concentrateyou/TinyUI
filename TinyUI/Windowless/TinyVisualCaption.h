#pragma once
#include "TinyVisual.h"

namespace TinyUI
{
	namespace Windowless
	{
		/// <summary>
		/// ±êÌâÀ¸ÈÝÆ÷¿Ø¼þ
		/// </summary>
		class TinyVisualCaption : public TinyVisual
		{
			friend class TinyVisualFactory;
			DECLARE_DYNAMIC(TinyVisualCaption);
			DISALLOW_COPY_AND_ASSIGN(TinyVisualCaption);
		protected:
			TinyVisualCaption(TinyVisual* spvisParent, TinyVisualTree* vtree);
		public:
			virtual ~TinyVisualCaption();
			TinyString RetrieveTag() const OVERRIDE;
		};
	}
}




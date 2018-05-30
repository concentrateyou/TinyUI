#pragma once
#include "TinyVisual.h"
#include "TinyVisualWindowless.h"

namespace TinyFramework
{
	namespace Windowless
	{
		/// <summary>
		/// ÏûÏ¢¿ò
		/// </summary>
		class TinyVisualMessageBox : public TinyVisualWindowless
		{
			friend class TinyVisualDocument;
			DECLARE_DYNAMIC(TinyVisualMessageBox)
			DISALLOW_COPY_AND_ASSIGN(TinyVisualMessageBox)
		public:
			TinyVisualMessageBox();
		};
	}
}




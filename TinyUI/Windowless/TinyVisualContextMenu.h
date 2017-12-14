#pragma once
#include "TinyVisual.h"
#include "TinyVisualWindowless.h"

namespace TinyUI
{
	namespace Windowless
	{
		/// <summary>
		/// ���ӻ������Ĳ˵�
		/// </summary>
		class TinyVisualContextMenu : public TinyVisualWindowless
		{
			friend class TinyVisualDocument;
			DECLARE_DYNAMIC(TinyVisualContextMenu)
			DISALLOW_COPY_AND_ASSIGN(TinyVisualContextMenu)
		};
	}
}




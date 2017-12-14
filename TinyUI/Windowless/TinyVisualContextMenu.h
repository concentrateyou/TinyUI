#pragma once
#include "TinyVisual.h"
#include "TinyVisualWindowless.h"

namespace TinyUI
{
	namespace Windowless
	{
		/// <summary>
		/// 可视化上下文菜单
		/// </summary>
		class TinyVisualContextMenu : public TinyVisualWindowless
		{
			friend class TinyVisualDocument;
			DECLARE_DYNAMIC(TinyVisualContextMenu)
			DISALLOW_COPY_AND_ASSIGN(TinyVisualContextMenu)
		};
	}
}




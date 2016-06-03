#pragma once
#include "../Control/TinyControl.h"
#include "../Render/TinyImage.h"
#include "TinyVisual.h"

namespace TinyUI
{
	namespace Windowless
	{
		class TinyVisualProperty
		{
		public:
			//‘™Àÿ Ù–‘
			static const TinyString NAME;
			static const TinyString TEXT;
			static const TinyString TOOLTIP;
			static const TinyString MAXSIZE;
			static const TinyString MINSIZE;
			static const TinyString SIZE;
			static const TinyString VISIBLE;
			static const TinyString ENABLE;
		};
		class TinyVisualTag
		{
		public:
			static const TinyString WINDOW;
			static const TinyString CAPTION;
			static const TinyString SYSBUTTON;
		};
	}
}




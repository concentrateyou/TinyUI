#pragma once
#include "../Common/TinyCollection.h"
#include "../Common/TinyString.h"
#include "../Common/TinyEvent.h"
#include "../Render/TinyCanvas.h"
#include "TinyVisual.h"
#include "TinyVisualCommon.h"
#include "TinyVisualEvent.h"

namespace TinyFramework
{
	namespace Windowless
	{
		/// <summary>
		/// ÔªËØ»ùÀà
		/// </summary>
		class TinyVisualElement : public TinyVisual
		{
			DECLARE_DYNCREATE(TinyVisualElement)
		protected:
			TinyVisualElement();
			TinyVisualElement(TinyVisual* spvisParent, TinyVisualDocument* document);
			virtual ~TinyVisualElement();
		public:
			
		};
	}
}




#include "../stdafx.h"
#include "TinyVisualDocument.h"
#include "TinyVisualWindowless.h"
#include "TinyVisualElement.h"

namespace TinyFramework
{
	namespace Windowless
	{
		IMPLEMENT_DYNCREATE(TinyVisualElement, TinyVisual);
		TinyVisualElement::TinyVisualElement()
		{

		}
		TinyVisualElement::TinyVisualElement(TinyVisual* spvisParent, TinyVisualDocument* document)
			:TinyVisual(spvisParent, document)
		{

		}
		TinyVisualElement::~TinyVisualElement()
		{

		}
	}
}

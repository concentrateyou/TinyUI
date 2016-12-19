#include "../stdafx.h"
#include "TinyStackPanel.h"
#include "TinyVisualDocument.h"

namespace TinyUI
{
	namespace Windowless
	{
		TinyStackPanel::~TinyStackPanel()
		{

		}
		TinyString TinyStackPanel::RetrieveTag() const
		{
			return TinyVisualTag::STACKPANEL;
		}
	}
}

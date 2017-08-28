#include "../stdafx.h"
#include "TinyVisualDocument.h"
#include "TinyVisualManage.h"
#include "TinyVisualPanel.h"
#include "TinyVisualHWND.h"

namespace TinyUI
{
	namespace Windowless
	{
		IMPLEMENT_DYNAMIC(TinyVisualPanel, TinyVisual);
		TinyVisualPanel::TinyVisualPanel(TinyVisual* spvisParent, TinyVisualDocument* vtree)
			:TinyVisual(spvisParent, vtree)
		{

		}
		TinyVisualPanel::~TinyVisualPanel()
		{

		}

		TinyString TinyVisualPanel::RetrieveTag() const
		{
			return TinyVisualTag::PANEL;
		}

		BOOL TinyVisualPanel::OnDraw(HDC hDC, const RECT& rcPaint)
		{
			return TRUE;
		}
	}
}

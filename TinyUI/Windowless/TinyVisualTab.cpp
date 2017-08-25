#include "../stdafx.h"
#include "TinyVisualDocument.h"
#include "TinyVisualManage.h"
#include "TinyVisualTab.h"

namespace TinyUI
{
	namespace Windowless
	{
		IMPLEMENT_DYNAMIC(TinyVisualTab, TinyVisual);
		TinyVisualTab::TinyVisualTab(TinyVisual* spvisParent, TinyVisualDocument* vtree)
			:TinyVisual(spvisParent, vtree)
		{

		}
		TinyVisualTab::~TinyVisualTab()
		{

		}
		TinyString TinyVisualTab::RetrieveTag() const
		{
			return TinyVisualTag::TAB;
		}

		BOOL TinyVisualTab::OnDraw(HDC hDC, const RECT& rcPaint)
		{
			TinyClipCanvas canvas(hDC, this, rcPaint);
			return TRUE;
		}
	}
}

#include "../stdafx.h"
#include "TinyVisualDocument.h"
#include "TinyVisualManage.h"
#include "TinyVisualTab.h"

namespace TinyFramework
{
	namespace Windowless
	{
		IMPLEMENT_DYNCREATE(TinyVisualTab, TinyVisual);

		TinyVisualTab::TinyVisualTab()
		{

		}
		TinyVisualTab::TinyVisualTab(TinyVisual* spvisParent, TinyVisualDocument* vtree)
			:TinyVisual(spvisParent, vtree)
		{

		}
		TinyVisualTab::~TinyVisualTab()
		{

		}
		TinyString TinyVisualTab::RetrieveTag() const
		{
			return TinyVisualTagConst::TAB;
		}

		BOOL TinyVisualTab::OnDraw(HDC hDC, const RECT& rcPaint)
		{
			ASSERT(m_document);
			TinyClipCanvas canvas(hDC, this, rcPaint);
			return TRUE;
		}
	}
}

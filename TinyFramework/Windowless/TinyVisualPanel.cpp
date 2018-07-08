#include "../stdafx.h"
#include "../Common/TinyString.h"
#include "TinyVisualCommon.h"
#include "TinyVisualManage.h"
#include "TinyVisualWindowless.h"
#include "TinyVisualPanel.h"

namespace TinyFramework
{
	namespace Windowless
	{
		IMPLEMENT_DYNCREATE(TinyVisualPanel, TinyVisual);

		TinyVisualPanel::TinyVisualPanel()
		{

		}
		TinyVisualPanel::TinyVisualPanel(TinyVisual* spvisParent, TinyVisualDocument* document)
			:TinyVisual(spvisParent, document)
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
			return TinyVisual::OnDraw(hDC, rcPaint);
		}
	}
}

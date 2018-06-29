#include "../stdafx.h"
#include "../Common/TinyString.h"
#include "TinyVisualCommon.h"
#include "TinyVisualManage.h"
#include "TinyVisualWindowless.h"
#include "TinyVisualButton.h"

namespace TinyFramework
{
	namespace Windowless
	{
		IMPLEMENT_DYNCREATE(TinyVisualButton, TinyVisual);

		TinyVisualButton::TinyVisualButton()
		{

		}
		TinyVisualButton::TinyVisualButton(TinyVisual* spvisParent, TinyVisualDocument* document)
			:TinyVisual(spvisParent, document)
		{

		}
		TinyVisualButton::~TinyVisualButton()
		{

		}


		TinyString TinyVisualWindow::RetrieveTag() const
		{
			return TinyVisualTag::BUTTON;
		}
		BOOL TinyVisualWindow::OnDraw(HDC hDC, const RECT& rcPaint)
		{
			return TinyVisual::OnDraw(hDC, rcPaint);
		}
	}
}

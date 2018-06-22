#include "../stdafx.h"
#include "../Common/TinyString.h"
#include "TinyVisualCommon.h"
#include "TinyVisualManage.h"
#include "TinyVisualWindowless.h"

namespace TinyFramework
{
	namespace Windowless
	{
		IMPLEMENT_DYNCREATE(TinyVisualWindow, TinyVisual);

		TinyVisualWindow::TinyVisualWindow()
		{

		}
		TinyVisualWindow::TinyVisualWindow(TinyVisual* spvisParent, TinyVisualDocument* document)
			:TinyVisual(spvisParent, document)
		{

		}
		TinyVisualWindow::~TinyVisualWindow()
		{

		}

		void TinyVisualWindow::SetPosition(const TinyPoint& pos)
		{
			TinyVisual::SetPosition(TinyPoint(0, 0));
		}

		void TinyVisualWindow::SetText(const TinyString& pzText)
		{
			ASSERT(m_document);
			::SetWindowText(m_document->GetVisualHWND().Handle(), pzText.CSTR());
			TinyVisual::SetText(pzText);
		}

		TinyString TinyVisualWindow::RetrieveTag() const
		{
			return TinyVisualTagConst::WINDOW;
		}
		BOOL TinyVisualWindow::OnDraw(HDC hDC, const RECT& rcPaint)
		{
			ASSERT(m_document);
			TinyClipCanvas canvas(hDC, this, rcPaint);
			TinyRectangle clip = m_document->GetWindowRect(this);
			return TRUE;
		}
	}
}

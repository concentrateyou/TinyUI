#include "../stdafx.h"
#include "../Common/TinyString.h"
#include "TinyVisualCommon.h"
#include "TinyVisualManage.h"
#include "TinyVisualWindowless.h"

namespace TinyFramework
{
	namespace Windowless
	{
		IMPLEMENT_DYNCREATE(TinyVisualWindow, TinyVisualElement);

		TinyVisualWindow::TinyVisualWindow()
		{

		}
		TinyVisualWindow::TinyVisualWindow(TinyVisual* spvisParent, TinyVisualDocument* document)
			:TinyVisualElement(spvisParent, document)
		{

		}
		TinyVisualWindow::~TinyVisualWindow()
		{

		}

		void TinyVisualWindow::SetPosition(const TinyPoint& pos)
		{
			TinyVisual::SetPosition(TinyPoint(0, 0));
			ASSERT(m_document);
			::SetWindowPos(m_document->GetVisualHWND().Handle(), NULL, pos.x, pos.y, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE);
		}

		void TinyVisualWindow::SetSize(const TinySize& size)
		{
			TinyVisual::SetSize(size);
			ASSERT(m_document);
			::SetWindowPos(m_document->GetVisualHWND().Handle(), NULL, 0, 0, size.cx, size.cy, SWP_NOACTIVATE | SWP_NOMOVE);
		}

		void TinyVisualWindow::SetText(const TinyString& pzText)
		{
			TinyVisual::SetText(pzText);
			ASSERT(m_document);
			::SetWindowText(m_document->GetVisualHWND().Handle(), pzText.CSTR());
		}

		TinyString TinyVisualWindow::RetrieveTag() const
		{
			return TinyVisualTag::WINDOW;
		}
		BOOL TinyVisualWindow::OnDraw(HDC hDC, const RECT& rcPaint)
		{
			ASSERT(m_document);
			TinyRectangle clip = m_document->GetWindowRect(this);
			TinyClipCanvas canvas(hDC, this, rcPaint);

			return TRUE;
		}
	}
}

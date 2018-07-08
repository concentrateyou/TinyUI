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
		void TinyVisualWindow::SetClip(HRGN hrgnClip)
		{
			TinyVisual::SetClip(hrgnClip);
			ASSERT(m_document);
			::SetWindowRgn(m_document->GetVisualHWND(), hrgnClip, TRUE);
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
			if (m_document->GetVisualHWND().RetrieveExStyle() & WS_EX_LAYERED)
			{
				canvas.SetBrush((HBRUSH)GetStockObject(NULL_BRUSH));
				canvas.FillRectangle(clip);
			}
			else
			{
				TinyBrush brush;
				brush.CreateBrush(m_backgroundColor);
				canvas.SetBrush(brush);
				canvas.FillRectangle(clip);
			}
			if (m_backgroundImage != NULL)
			{
				if (m_backgroundCenter.IsRectEmpty())
				{
					canvas.DrawImage(*m_backgroundImage, clip, m_backgroundRectangle);
				}
				else
				{
					canvas.DrawImage(*m_backgroundImage, clip, m_backgroundRectangle, m_backgroundCenter);
				}
			}
			return TRUE;
		}
	}
}

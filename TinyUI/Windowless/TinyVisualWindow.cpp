#include "../stdafx.h"
#include "../Common/TinyString.h"
#include "TinyVisualCommon.h"
#include "TinyVisualManage.h"
#include "TinyVisualHWND.h"

namespace TinyUI
{
	namespace Windowless
	{
		IMPLEMENT_DYNAMIC(TinyVisualWindow, TinyVisual);

		TinyVisualWindow::TinyVisualWindow(TinyVisual* spvisParent, TinyVisualDocument* document)
			:TinyVisual(spvisParent, document)
		{

		}
		TinyVisualWindow::~TinyVisualWindow()
		{

		}

		void TinyVisualWindow::OnSizeChange(const TinySize& oldsize, const TinySize& newsize)
		{
			TinyVisual* spvis = m_spvisChild;
			while (spvis != NULL && spvis->IsVisible())
			{
				TinySize size = spvis->GetSize();
				spvis->SetSize(TinySize(newsize.cx, size.cy));
				spvis->OnSizeChange(size, spvis->GetSize());
				spvis = m_document->GetVisual(spvis, CMD_NEXT);
			}
		}

		void TinyVisualWindow::SetPosition(const TinyPoint& pos)
		{
			TinyVisual::SetPosition(TinyPoint(0, 0));
		}

		TinyString TinyVisualWindow::RetrieveTag() const
		{
			return TinyVisualTag::WINDOW;
		}
		BOOL TinyVisualWindow::IsLayout() const
		{
			return TRUE;
		}
		BOOL TinyVisualWindow::OnDraw(HDC hDC, const RECT& rcPaint)
		{
			ASSERT(m_document || m_document->GetVisualHWND());
			TinyClipCanvas canvas(hDC, this, rcPaint);
			TinyRectangle clip = m_document->GetWindowRect(this);
			if (!m_backgroundColor.IsEmpty())
			{
				TinyBrush brush;
				brush.CreateBrush(m_backgroundColor);
				HBRUSH hOldBrush = canvas.SetBrush(brush);
				canvas.FillRectangle(clip);
				canvas.SetBrush(hOldBrush);
			}
			if (!m_borderColor.IsEmpty() && m_borderThickness != -1)
			{
				TinyPen pen;
				pen.CreatePen(m_borderStyle, m_borderThickness, m_borderColor);
				HPEN hOldPen = canvas.SetPen(pen);
				canvas.DrawRectangle(clip);
				canvas.SetPen(hOldPen);
			}
			if (!m_backgroundImage.IsEmpty())
			{
				TinyRectangle srcRect = m_backgroundImage.GetRectangle();
				TinyRectangle srcCenter = GetBackgroundCenter();
				if (srcCenter.IsRectEmpty())
				{
					canvas.DrawImage(m_backgroundImage, clip, srcRect);
				}
				else
				{
					canvas.DrawImage(m_backgroundImage, clip, srcRect, srcCenter);
				}
			}
			return TRUE;
		}
	}
}

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
			if (!m_backgroundImage.IsEmpty())
			{
				TinyRectangle srcRect = m_backgroundImage.GetRectangle();
				TinyRectangle srcCenter = GetBackgroundCenter();
				if (srcCenter.IsRectEmpty())
				{
					canvas.DrawImage(m_backgroundImage, m_rectangle, srcRect);
				}
				else
				{
					canvas.DrawImage(m_backgroundImage, m_rectangle, srcRect, srcCenter);
				}
			}
			else
			{
				canvas.SetBrush((HBRUSH)GetStockObject(WHITE_BRUSH));
				canvas.FillRectangle(m_rectangle);
			}
			return TRUE;
		}
	}
}

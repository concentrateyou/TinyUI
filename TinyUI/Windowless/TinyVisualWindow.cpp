#include "../stdafx.h"
#include "../Common/TinyString.h"
#include "TinyVisualCommon.h"
#include "TinyVisualManage.h"
#include "TinyVisualHWND.h"

namespace TinyUI
{
	namespace Windowless
	{
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

		HRESULT TinyVisualWindow::SetProperty(const TinyString& name, const TinyString& value)
		{
			if (strcasecmp(name.STR(), TinyVisualProperty::BACKGROUNDIMAGE.STR()) == 0)
			{
				m_backgroundImage.Load(value.STR());
			}
			return TinyVisual::SetProperty(name, value);
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
				canvas.DrawImage(m_backgroundImage, m_rectangle, 0, 0, m_backgroundImage.GetSize().cx, m_backgroundImage.GetSize().cy);
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

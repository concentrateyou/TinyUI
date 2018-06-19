#include "../stdafx.h"
#include "TinyVisualDocument.h"
#include "TinyVisualManage.h"
#include "TinyVisualPanel.h"
#include "TinyVisualWindowless.h"

namespace TinyFramework
{
	namespace Windowless
	{
		IMPLEMENT_DYNCREATE(TinyVisualPanel, TinyVisual);

		TinyVisualPanel::TinyVisualPanel()
			:m_layout(None)
		{

		}
		TinyVisualPanel::TinyVisualPanel(TinyVisual* spvisParent, TinyVisualDocument* vtree)
			: TinyVisual(spvisParent, vtree),
			m_layout(None)
		{

		}
		TinyVisualPanel::~TinyVisualPanel()
		{

		}
		TinyString TinyVisualPanel::RetrieveTag() const
		{
			return TinyVisualTagConst::PANEL;
		}

		BOOL TinyVisualPanel::SetProperty(const TinyString& name, const TinyString& value)
		{
			if (strcasecmp(name.STR(), TinyVisualPropertyConst::LAYOUT.STR()) == 0)
			{
				if (strcasecmp(value.STR(), "Horizontal") == 0)
				{
					m_layout = Horizontal;
				}
				if (strcasecmp(value.STR(), "Vertical") == 0)
				{
					m_layout = Vertical;
				}
				return TRUE;
			}
			return TinyVisual::SetProperty(name, value);
		}

		VisualLayout TinyVisualPanel::GetLayout() const
		{
			return m_layout;
		}

		void TinyVisualPanel::OnSizeChange(const TinySize& _old, const TinySize& _new)
		{
			//TinyVisual* spvis = m_document->GetVisual(this, CMD_CHILD);
			//DWORD count = spvis->GetChildCount();
			//spvis = m_document->GetVisual(spvis, CMD_FIRST);
			//while (spvis != NULL)
			//{
			//	if (!spvis->IsVisible())
			//		continue;
			//	if (m_layout == Horizontal)//水平
			//	{
			//		INT x = 0;
			//		TinyPoint position = spvis->GetPosition();
			//		if (position.IsEmpty())
			//		{
			//			TinySize minimumSize = spvis->GetMinimumSize();
			//			TinySize maximumSize = spvis->GetMaximumSize();
			//			TinySize size = spvis->GetSize();
			//			if (size.cx == 0)
			//			{

			//			}
			//			else
			//			{

			//			}
			//			size.cy = size.cy == 0 ? _new.cy : size.cy;
			//			switch (spvis->GetAlignment())
			//			{
			//			case Alignment::LEFT:
			//			{
			//				spvis->SetPosition(TinyPoint(x, 0));
			//			}
			//			break;
			//			case Alignment::RIGHT:
			//			{
			//				spvis->SetPosition(TinyPoint(_new.cx - x, 0));
			//			}
			//			break;
			//			}
			//		}
			//	}
			//	spvis = m_document->GetVisual(spvis, CMD_NEXT);
			//}
		}

		BOOL TinyVisualPanel::OnDraw(HDC hDC, const RECT& rcPaint)
		{
			ASSERT(m_document);
			TinyClipCanvas canvas(hDC, this, rcPaint);
			TinyRectangle clip = m_document->GetWindowRect(this);
			if (m_backgroundImage != NULL && !m_backgroundImage->IsEmpty())
			{
				if (!m_backgroundCenter.IsRectEmpty())
				{
					canvas.DrawImage(*m_backgroundImage, clip, m_backgroundImage->GetRectangle(), m_backgroundCenter);
				}
				else
				{
					canvas.DrawImage(*m_backgroundImage, clip, 0, 0, m_backgroundImage->GetSize().cx, m_backgroundImage->GetSize().cy);
				}
			}
			if (!m_backgroundColor.IsTransparent())
			{
				TinyBrush brush;
				brush.CreateBrush(m_backgroundColor);
				canvas.SetBrush(brush);
				canvas.FillRectangle(clip);
			}
			if (!m_borderColor.IsTransparent() && m_borderThickness >= 1)
			{
				TinyPen pen;
				pen.CreatePen(m_borderStyle, m_borderThickness, m_borderColor);
				canvas.SetBrush((HBRUSH)GetStockObject(NULL_BRUSH));
				canvas.SetPen(pen);
				canvas.DrawRectangle(clip);
			}
			return TRUE;
		}
	}
}

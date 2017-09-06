#include "../stdafx.h"
#include "TinyVisualDocument.h"
#include "TinyVisualManage.h"
#include "TinyVisualPanel.h"
#include "TinyVisualHWND.h"

namespace TinyUI
{
	namespace Windowless
	{
		IMPLEMENT_DYNAMIC(TinyVisualPanel, TinyVisual);
		TinyVisualPanel::TinyVisualPanel(TinyVisual* spvisParent, TinyVisualDocument* vtree)
			:TinyVisual(spvisParent, vtree)
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
			ASSERT(m_document || m_document->GetVisualHWND());
			TinyClipCanvas canvas(hDC, this, rcPaint);
			TinyRectangle clip = m_document->GetWindowRect(this);
			if (m_backgroundImage != NULL && !m_backgroundImage->IsEmpty())
			{
				canvas.DrawImage(*m_backgroundImage, clip, 0, 0, m_backgroundImage->GetSize().cx, m_backgroundImage->GetSize().cy);
			}
			if (!m_borderColor.IsEmpty() && m_borderThickness != -1)
			{
				TinyPen pen;
				pen.CreatePen(m_borderStyle, m_borderThickness, m_borderColor);
				canvas.SetBrush((HBRUSH)GetStockObject(NULL_BRUSH));
				canvas.SetPen(pen);
				canvas.DrawRectangle(clip);
			}
			if (!m_backgroundColor.IsEmpty())
			{
				TinyBrush brush;
				brush.CreateBrush(m_backgroundColor);
				canvas.SetBrush(brush);
				canvas.FillRectangle(clip);
			}
			return TRUE;
		}
	}
}

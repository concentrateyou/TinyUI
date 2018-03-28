#include "../stdafx.h"
#include "TinyVisualDocument.h"
#include "TinyVisualManage.h"
#include "TinyVisualPanel.h"
#include "TinyVisualWindowless.h"

namespace TinyUI
{
	namespace Windowless
	{
		IMPLEMENT_DYNCREATE(TinyVisualPanel, TinyVisual);

		TinyVisualPanel::TinyVisualPanel()
		{

		}
		TinyVisualPanel::TinyVisualPanel(TinyVisual* spvisParent, TinyVisualDocument* vtree)
			:TinyVisual(spvisParent, vtree)
		{

		}
		TinyVisualPanel::~TinyVisualPanel()
		{

		}
		TinyString TinyVisualPanel::RetrieveTag() const
		{
			return TinyVisualTagConst::PANEL;
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
			if (!m_borderColor.IsTransparent() && m_borderThickness != -1)
			{
				TinyPen pen;
				pen.CreatePen(m_borderStyle, m_borderThickness, m_borderColor);
				canvas.SetBrush((HBRUSH)GetStockObject(NULL_BRUSH));
				canvas.SetPen(pen);
				canvas.DrawRectangle(clip);
			}
			return TRUE;
		}

		//////////////////////////////////////////////////////////////////////////
		IMPLEMENT_DYNCREATE(TinyVisualStackPanel, TinyVisual);
		TinyVisualStackPanel::TinyVisualStackPanel()
		{

		}
		TinyVisualStackPanel::~TinyVisualStackPanel()
		{

		}
		TinyString TinyVisualStackPanel::RetrieveTag() const
		{
			return TinyVisualTagConst::PANEL;
		}

		BOOL TinyVisualStackPanel::SetProperty(const TinyString& name, const TinyString& value)
		{
			/*if (strcasecmp(name.STR(), TinyVisualPropertyConst::IMAGENORMAL.STR()) == 0)
			{
				
				return TRUE;
			}*/
			return TinyVisualPanel::SetProperty(name, value);
		}

	}
}

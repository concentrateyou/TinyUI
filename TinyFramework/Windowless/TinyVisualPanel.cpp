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
			:m_orientation(None)
		{

		}
		TinyVisualPanel::TinyVisualPanel(TinyVisual* spvisParent, TinyVisualDocument* vtree)
			: TinyVisual(spvisParent, vtree),
			m_orientation(None)
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
			if (strcasecmp(name.STR(), TinyVisualPropertyConst::ORIENTATION.STR()) == 0)
			{
				if (strcasecmp(value.STR(), "Horizontal") == 0)
				{
					m_orientation = Horizontal;
				}
				if (strcasecmp(value.STR(), "Vertical") == 0)
				{
					m_orientation = Vertical;
				}
				if (strcasecmp(name.STR(), TinyVisualPropertyConst::HORIZONTALALIGNMENT.STR()) == 0)
				{
					m_horizontalAlignment = TinyVisualBuilder::GetHorizontalAlignment(value);
				}
				if (strcasecmp(name.STR(), TinyVisualPropertyConst::VERTICALALIGNMENT.STR()) == 0)
				{
					m_verticalAlignment = TinyVisualBuilder::GetVerticalAlignment(value);
				}
				return TRUE;
			}
			return TinyVisual::SetProperty(name, value);
		}

		Orientation TinyVisualPanel::GetOrientation() const
		{
			return m_orientation;
		}

		HorizontalAlignment	TinyVisualPanel::GetHorizontalAlignment() const
		{
			return m_horizontalAlignment;
		}

		VerticalAlignment	TinyVisualPanel::GetVerticalAlignment() const
		{
			return m_verticalAlignment;
		}

		void TinyVisualPanel::OnSizeChange(const TinySize& _old, const TinySize& _new)
		{

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

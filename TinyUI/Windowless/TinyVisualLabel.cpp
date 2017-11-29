#include "../stdafx.h"
#include "TinyVisualDocument.h"
#include "TinyVisualManage.h"
#include "TinyVisualLabel.h"

namespace TinyUI
{
	namespace Windowless
	{
		IMPLEMENT_DYNCREATE(TinyVisualLabel, TinyVisual);
		
		TinyVisualLabel::TinyVisualLabel()
		{

		}
		TinyVisualLabel::TinyVisualLabel(TinyVisual* spvisParent, TinyVisualDocument* vtree)
			:TinyVisual(spvisParent, vtree)
		{

		}
		TinyVisualLabel::~TinyVisualLabel()
		{

		}
		TinyString TinyVisualLabel::RetrieveTag() const
		{
			return TinyVisualTag::LABEL;
		}

		BOOL TinyVisualLabel::OnDraw(HDC hDC, const RECT& rcPaint)
		{
			if (!m_document)
				return FALSE;
			TinyClipCanvas canvas(hDC, this, rcPaint);
			TinyRectangle clip = m_document->GetWindowRect(this);
			if (!m_backgroundColor.IsEmpty())
			{
				TinyBrush brush;
				brush.CreateBrush(m_backgroundColor);
				canvas.SetBrush(brush);
				canvas.FillRectangle(clip);
			}
			if (!m_borderColor.IsEmpty() && m_borderThickness != -1)
			{
				TinyPen pen;
				pen.CreatePen(m_borderStyle, m_borderThickness, m_borderColor);
				canvas.SetPen(pen);
				canvas.DrawRectangle(clip);
			}
			if (m_backgroundImage != NULL && !m_backgroundImage->IsEmpty())
			{
				TinyRectangle srcRect = m_backgroundImage->GetRectangle();
				TinyRectangle srcCenter = GetBackgroundCenter();
				if (srcCenter.IsRectEmpty())
				{
					canvas.DrawImage(*m_backgroundImage, clip, srcRect);
				}
				else
				{
					canvas.DrawImage(*m_backgroundImage, clip, srcRect, srcCenter);
				}
			}
			canvas.SetFont(m_hFONT);
			canvas.SetTextColor(m_textColor);
			canvas.DrawString(m_szText, clip, m_textAlign);
			return TRUE;
		}
	}
}

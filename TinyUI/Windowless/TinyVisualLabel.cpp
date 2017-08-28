#include "../stdafx.h"
#include "TinyVisualDocument.h"
#include "TinyVisualManage.h"
#include "TinyVisualLabel.h"

namespace TinyUI
{
	namespace Windowless
	{
		IMPLEMENT_DYNAMIC(TinyVisualLabel, TinyVisual);
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
			TinyClipCanvas canvas(hDC, this, rcPaint);
			TinyRectangle clip = m_document->GetWindowRect(this);
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
			INT oldColor = -1;
			HFONT hOldFONT = NULL;
			if (m_hFONT != NULL)
			{
				hOldFONT = canvas.SetFont(m_hFONT);
			}
			if (!m_textColor.IsEmpty())
			{
				oldColor = canvas.SetTextColor(m_textColor);
			}
			canvas.DrawString(m_szText, clip, m_textAlign);
			if (hOldFONT != NULL)
				canvas.SetFont(hOldFONT);
			if (oldColor != -1)
				canvas.SetTextColor(oldColor);
			return TRUE;
		}
	}
}

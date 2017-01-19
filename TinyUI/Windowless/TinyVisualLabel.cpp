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
			HFONT hOldFONT = canvas.SetFont(m_hFONT);
			COLORREF oldColor = canvas.SetTextColor(m_textColor);
			TinyRectangle clip = m_document->GetWindowRect(this);
			canvas.DrawString(m_strText, clip, m_textAlign);
			canvas.SetFont(hOldFONT);
			canvas.SetTextColor(oldColor);
			return TRUE;
		}
	}
}

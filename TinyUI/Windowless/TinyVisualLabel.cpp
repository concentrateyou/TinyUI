#include "../stdafx.h"
#include "TinyVisualTree.h"
#include "TinyVisualManage.h"
#include "TinyVisualLabel.h"

namespace TinyUI
{
	namespace Windowless
	{
		TinyVisualLabel::TinyVisualLabel(TinyVisual* spvisParent, TinyVisualTree* vtree)
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
			if (canvas.IsValid())
			{
				canvas.SetFont((HFONT)GetStockObject(DEFAULT_GUI_FONT));
				TinyRectangle clip = m_vtree->GetWindowRect(this);
				canvas.DrawString(m_strText, clip, DT_LEFT);
				return TRUE;
			}
			return FALSE;
		}
	}
}

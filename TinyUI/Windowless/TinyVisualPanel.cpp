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
			if (!m_backgroundImage.IsEmpty())
			{
				canvas.DrawImage(m_backgroundImage, m_rectangle, 0, 0, m_backgroundImage.GetSize().cx, m_backgroundImage.GetSize().cy);
			}
			return TRUE;
		}
	}
}

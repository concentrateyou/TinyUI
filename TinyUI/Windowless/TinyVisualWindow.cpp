#include "../stdafx.h"
#include "../Common/TinyString.h"
#include "TinyVisualCommon.h"
#include "TinyVisualManage.h"
#include "TinyVisualHWND.h"

namespace TinyUI
{
	namespace Windowless
	{
		TinyVisualWindow::TinyVisualWindow(TinyVisual* spvisParent, TinyVisualTree* vtree)
			:TinyVisual(spvisParent, vtree)
		{
			TinyVisualHWND* pWND = vtree->GetVisualHWND();
			if (pWND)
			{
				pWND->GetClientRect(&m_windowRect);
			}
		}
		TinyVisualWindow::~TinyVisualWindow()
		{

		}
		TinyString TinyVisualWindow::RetrieveTag() const
		{
			return TinyVisualTag::WINDOW;
		}
		HRESULT TinyVisualWindow::OnDraw(HDC hDC)
		{
			ASSERT(m_vtree || m_vtree->GetVisualHWND());
			TinyImage& image = m_images[NORMAL];
			if (image.IsEmpty()) return S_FALSE;
			TinyCanvas canvas(hDC);
			TinyCanvasClip clip(canvas, this);
			canvas.SetBrush((HBRUSH)GetStockObject(WHITE_BRUSH));
			canvas.FillRectangle(m_windowRect);
			canvas.DrawImage(image, m_windowRect, 0, 0, image.GetSize().cx, image.GetSize().cy);
		}
	}
}

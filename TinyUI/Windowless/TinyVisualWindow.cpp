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
				pWND->GetWindowRect(&m_windowRect);
			}
		}
		TinyVisualWindow::~TinyVisualWindow()
		{

		}
		TinyString TinyVisualWindow::RetrieveTag() const
		{
			return TinyVisualTag::WINDOW;
		}
		HRESULT TinyVisualWindow::OnDraw(TinyCanvas& canvas)
		{
			ASSERT(m_vtree || m_vtree->GetVisualHWND());
			TinyImage& image = m_images[NORMAL];
			if (image.IsEmpty()) return S_FALSE;
			canvas.SetBrush((HBRUSH)GetStockObject(WHITE_BRUSH));
			TinyRectangle rectangle;
			m_vtree->GetVisualHWND()->GetClientRect(&rectangle);
			canvas.FillRectangle(rectangle);
			canvas.DrawImage(image, rectangle, 0, 0, image.GetSize().cx, image.GetSize().cy);
			TinyVisual* spvis = m_vtree->GetVisual(this, CMD_CHILD);
			while (spvis)
			{
				spvis->OnDraw(canvas);
				spvis = m_vtree->GetVisual(spvis, CMD_CHILD);
			}
			return S_OK;
		}

		HRESULT TinyVisualWindow::OnMouseMove(POINT pos)
		{
			return FALSE;
		}

		HRESULT TinyVisualWindow::OnLButtonDown(POINT pos)
		{
			return FALSE;
		}

		HRESULT TinyVisualWindow::OnLButtonUp(POINT pos)
		{
			return FALSE;
		}

		HRESULT TinyVisualWindow::OnRButtonDown(POINT pos)
		{
			return FALSE;
		}

		HRESULT TinyVisualWindow::OnRButtonUp(POINT pos)
		{
			return FALSE;
		}
	}
}

#include "../stdafx.h"
#include "TinyVisualWindow.h"

namespace TinyUI
{
	namespace Windowless
	{
		TinyVisualWindow::TinyVisualWindow(TinyVisualTree* pT)
			:m_pT(pT)
		{

		}
		LPCSTR TinyVisualWindow::RetrieveTag()
		{
			return TEXT("window");
		}

		HRESULT TinyVisualWindow::OnDraw(TinyDC& dc, TinyRectangle& drawRect)
		{
			return FALSE;
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

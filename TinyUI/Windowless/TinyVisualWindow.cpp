#include "../stdafx.h"
#include "../Common/TinyString.h"
#include "TinyVisualManage.h"
#include "TinyVisualHWND.h"

namespace TinyUI
{
	namespace Windowless
	{
		TinyVisualWindow::TinyVisualWindow(TinyVisualTree* myT)
			:TinyVisual(NULL), m_myT(myT)
		{

		}
		TinyVisualWindow::~TinyVisualWindow()
		{

		}
		LPCSTR TinyVisualWindow::RetrieveTag()
		{
			return TinyVisualTag::WINDOW;
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

		void TinyVisualWindow::OnSizeChange(const TinySize&oldSize, const TinySize&newSize)
		{
		}
	}
}

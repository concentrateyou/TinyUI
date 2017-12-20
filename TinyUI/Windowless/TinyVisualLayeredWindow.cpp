#include "../stdafx.h"
#include "TinyVisualDocument.h"
#include "TinyVisualManage.h"
#include "TinyVisualLayeredWindow.h"

namespace TinyUI
{
	namespace Windowless
	{
		TinyVisualLayeredWindow::TinyVisualLayeredWindow()
		{

		}
		TinyVisualLayeredWindow::~TinyVisualLayeredWindow()
		{

		}

		LPCSTR TinyVisualLayeredWindow::RetrieveClassName()
		{
			return TEXT("TinyVisualLayeredWindow");
		}

		LPCSTR TinyVisualLayeredWindow::RetrieveTitle()
		{
			return TEXT("TinyVisualLayeredWindow");
		}

		HICON TinyVisualLayeredWindow::RetrieveIcon()
		{
			return NULL;
		}

		DWORD TinyVisualLayeredWindow::RetrieveStyle()
		{
			return (WS_VISIBLE | WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_MINIMIZEBOX);
		}

		DWORD TinyVisualLayeredWindow::RetrieveExStyle()
		{
			return (WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);
		}
		void TinyVisualLayeredWindow::OnInitialize()
		{

		}
		void TinyVisualLayeredWindow::OnUninitialize()
		{

		}
		BOOL TinyVisualLayeredWindow::Update()
		{
			if (!m_hWND || !m_visualDC)
				return FALSE;
			TinyRectangle windowRect;
			::GetWindowRect(m_hWND, &windowRect);
			TinyRectangle rectangle = windowRect;
			rectangle.OffsetRect(-rectangle.left, -rectangle.top);
			m_document.Draw(m_visualDC, rectangle);
			TinyPoint pos;
			TinyPoint dstPos = windowRect.Position();
			TinySize  dstSize = windowRect.Size();
			BLENDFUNCTION bs = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
			BOOL bRes = ::UpdateLayeredWindow(m_hWND, m_visualDC->Handle(), &dstPos, &dstSize, m_visualDC->GetMemDC(), &pos, 0, &bs, 2);
			return bRes;
		}
	}
}

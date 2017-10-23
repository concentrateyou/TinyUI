#include "stdafx.h"
#include "MSearchWindow.h"

namespace MShow
{
	IMPLEMENT_DYNAMIC(MSearchWindow, TinyVisualHWND);

	MSearchWindow::MSearchWindow()
	{
	}

	MSearchWindow::~MSearchWindow()
	{
	}

	LPCSTR MSearchWindow::RetrieveClassName()
	{
		return TEXT("MSearchWindow");
	}

	LPCSTR MSearchWindow::RetrieveTitle()
	{
		return TEXT("MSearchWindow");
	}

	void MSearchWindow::OnInitialize()
	{
		
	}

	void MSearchWindow::OnUninitialize()
	{

	}
}



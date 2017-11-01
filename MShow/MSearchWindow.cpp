#include "stdafx.h"
#include "MSearchWindow.h"
#include "resource.h"

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

	HICON MSearchWindow::RetrieveIcon()
	{
		return LoadIcon(TinyApplication::GetInstance()->Handle(), MAKEINTRESOURCE(IDI_MSHOW));
	}

	void MSearchWindow::OnInitialize()
	{

	}

	void MSearchWindow::OnUninitialize()
	{

	}
}



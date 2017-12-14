#include "stdafx.h"
#include "resource.h"
#include "MClientWindow.h"
#include "Media/TinyWASAPIAudioCapture.h"
using namespace TinyUI::Media;

namespace MShow
{
	IMPLEMENT_DYNAMIC(MClientWindow, TinyVisualWindowless);

	MClientWindow::MClientWindow()
	{

	}
	MClientWindow::~MClientWindow()
	{
	}


	LPCSTR MClientWindow::RetrieveClassName()
	{
		return TEXT("MClientWindow");
	}

	LPCSTR MClientWindow::RetrieveTitle()
	{
		return TEXT("MClientWindow");
	}

	HICON MClientWindow::RetrieveIcon()
	{
		return LoadIcon(TinyApplication::GetInstance()->Handle(), MAKEINTRESOURCE(IDI_MSHOW));
	}
	void MClientWindow::OnInitialize()
	{
		m_previewView.Create(m_hWND, 0, 0, 0, 0);
	}

	void MClientWindow::OnUninitialize()
	{

	}
}
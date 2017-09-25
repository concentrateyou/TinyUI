#include "stdafx.h"
#include "MClientWindow.h"
#include "Media/TinyWASAPIAudioCapture.h"
using namespace TinyUI::Media;

namespace MShow
{
	IMPLEMENT_DYNAMIC(MClientWindow, TinyVisualHWND);

	MClientWindow::MClientWindow()
	{

	}
	MClientWindow::~MClientWindow()
	{
	}

	void MClientWindow::OnInitialize()
	{
		m_previewView.Create(m_hWND, 0, 0, 0, 0);
	}

	void MClientWindow::OnUninitialize()
	{

	}

}
#include "stdafx.h"
#include "MShowWindow.h"
#include "Media/TinyWASAPIAudioCapture.h"
using namespace TinyUI::Media;

namespace MShow
{
	IMPLEMENT_DYNAMIC(MShowWindow, TinyVisualHWND);

	MShowWindow::MShowWindow()
	{

	}
	MShowWindow::~MShowWindow()
	{
	}

	void MShowWindow::OnInitialize()
	{
		m_previewView.Create(m_hWND, 0, 0, 0, 0);
		m_analyserBAR.Create(m_hWND, 0, 0, 0, 0, FALSE);
	}

	void MShowWindow::OnUninitialize()
	{

	}
}
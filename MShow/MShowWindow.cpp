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
	LPCSTR MShowWindow::RetrieveClassName()
	{
		return TEXT("MShowWindow");
	}

	LPCSTR MShowWindow::RetrieveTitle()
	{
		return TEXT("MShowWindow");
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
#include "stdafx.h"
#include "MainWindowless.h"

namespace Bytedance
{
	IMPLEMENT_DYNAMIC(MainWindowless, TinyVisualWindowless)

	MainWindowless::MainWindowless()
	{
	}


	MainWindowless::~MainWindowless()
	{
	}

	void MainWindowless::OnInitialize()
	{
		TinyVisualWindow* window = static_cast<TinyVisualWindow*>(m_document.GetParent(NULL));
		window->SetMinimumSize(TinySize(800, 600));
		window->SetSize(TinySize(800, 600));
		window->SetPosition(TinySize(100, 100));
	}

	void MainWindowless::OnUninitialize()
	{
		
	}

}

#include "stdafx.h"
#include "SkinWindow.h"


IMPLEMENT_DYNAMIC(SkinWindow, TinyVisualWindowless)
SkinWindow::SkinWindow()
{

}


SkinWindow::~SkinWindow()
{
}

void SkinWindow::OnInitialize()
{
	BuildUI();
}

void SkinWindow::OnUninitialize()
{

}

BOOL SkinWindow::BuildUI()
{
	TinyVisualWindow* window = static_cast<TinyVisualWindow*>(m_document.GetParent(NULL));
	window->SetMinimumSize(TinySize(800, 600));
	window->SetSize(TinySize(800, 600));
	window->SetPosition(TinySize(100, 100));
}

void SkinWindow::Resize(const TinySize& size)
{

}

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
	//m_document.GetParent(NULL)->SetMaximumSize(TinySize(800, 800));
	//m_document.GetParent(NULL)->SetMinimumSize(TinySize(600, 600));
	//m_document.GetParent(NULL)->SetSize(TinySize(600, 600));
	//m_document.GetParent(NULL)->SetPosition(TinySize(100, 100));
}

void SkinWindow::OnUninitialize()
{

}


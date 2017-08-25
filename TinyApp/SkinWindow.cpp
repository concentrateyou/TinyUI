#include "stdafx.h"
#include "SkinWindow.h"

IMPLEMENT_DYNAMIC(SkinWindow, TinyVisualHWND)
SkinWindow::SkinWindow()
{
}


SkinWindow::~SkinWindow()
{
}


void SkinWindow::OnInitialize()
{
	TinyVisual* sysmin = GetDocument()->GetVisualByName("sysmin");
	if (sysmin != NULL)
	{
		m_onMinClick.Reset(new Delegate<void(EventArgs&)>(this, &SkinWindow::OnMinClick));
		sysmin->EVENT_CLICK += m_onMinClick;
	}
}

void SkinWindow::OnUninitialize()
{
	TinyVisual* sysmin = GetDocument()->GetVisualByName("sysmin");
	if (sysmin != NULL)
	{
		sysmin->EVENT_CLICK -= m_onMinClick;
	}
}

void SkinWindow::OnMinClick(EventArgs& args)
{
	SendMessage(m_hWND, WM_SYSCOMMAND, SC_MINIMIZE, 0);
	//ShowWindow(SW_MINIMIZE);
}

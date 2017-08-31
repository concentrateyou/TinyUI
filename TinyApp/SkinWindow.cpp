#include "stdafx.h"
#include "SkinWindow.h"

IMPLEMENT_DYNAMIC(SkinWindow, TinyVisualHWND)
SkinWindow::SkinWindow()
{
	strncmp("", "audio", 5);
}


SkinWindow::~SkinWindow()
{
}


void SkinWindow::OnInitialize()
{
	TinyVisual* visual = GetDocument()->GetVisualByName("sysmin");
	if (visual != NULL)
	{
		m_onMinimumClick.Reset(new Delegate<void(EventArgs&)>(this, &SkinWindow::OnMinimumClick));
		visual->EVENT_CLICK += m_onMinimumClick;
	}
	visual = GetDocument()->GetVisualByName("sysclose");
	if (visual != NULL)
	{
		m_onCloseClick.Reset(new Delegate<void(EventArgs&)>(this, &SkinWindow::OnCloseClick));
		visual->EVENT_CLICK += m_onCloseClick;
	}
	visual = GetDocument()->GetVisualByName("audio");
	if (visual != NULL)
	{
		if (visual->IsKindOf(RUNTIME_CLASS(TinyVisualComboBox)))
		{
			TinyVisualComboBox* combobox = static_cast<TinyVisualComboBox*>(visual);
			for (INT i = 0;i < 50;i++)
			{
				TinyString str = TinyString::Format("Name%d", i);
				combobox->AddOption(str, str);
			}
		}
	}
}

void SkinWindow::OnUninitialize()
{
	TinyVisual* visual = GetDocument()->GetVisualByName("sysmin");
	if (visual != NULL)
	{
		visual->EVENT_CLICK -= m_onMinimumClick;
	}
	visual = GetDocument()->GetVisualByName("sysclose");
	if (visual != NULL)
	{
		visual->EVENT_CLICK -= m_onCloseClick;
	}
}

void SkinWindow::OnMinimumClick(EventArgs& args)
{
	GetDocument()->ReleaseCapture();//±ÿ–Î Õ∑≈≤∂ªÒ
	SendMessage(m_hWND, WM_SYSCOMMAND, SC_MINIMIZE, NULL);
}

void SkinWindow::OnCloseClick(EventArgs& args)
{
	SendMessage(m_hWND, WM_CLOSE, NULL, NULL);
}

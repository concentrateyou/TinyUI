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
			m_onSelectChanged.Reset(new Delegate<void(TinyVisualOption*)>(this, &SkinWindow::OnSelectChanged));
			combobox->EVENT_SELECTCHANGED += m_onSelectChanged;
			for (INT i = 0;i < 10;i++)
			{
				TinyString str = TinyString::Format("Name%d", i);
				TinyVisualOption* option = combobox->AddOption(str, str);
				if (i == 5)
				{
					combobox->SetSelected(option);
				}
			}
		}
	}
	visual = GetDocument()->GetVisualByName("test");
	if (visual != NULL)
	{
		m_onButtonClick.Reset(new Delegate<void(EventArgs&)>(this, &SkinWindow::OnButtonClick));
		visual->EVENT_CLICK += m_onButtonClick;
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
	visual = GetDocument()->GetVisualByName("audio");
	if (visual != NULL)
	{
		if (visual->IsKindOf(RUNTIME_CLASS(TinyVisualComboBox)))
		{
			TinyVisualComboBox* combobox = static_cast<TinyVisualComboBox*>(visual);
			combobox->EVENT_SELECTCHANGED -= m_onSelectChanged;
		}
	}
	visual = GetDocument()->GetVisualByName("test");
	if (visual != NULL)
	{
		m_onButtonClick.Reset(new Delegate<void(EventArgs&)>(this, &SkinWindow::OnButtonClick));
		visual->EVENT_CLICK -= m_onButtonClick;
	}
}

void SkinWindow::OnMinimumClick(EventArgs& args)
{
	GetDocument()->ReleaseCapture();//必须释放捕获
	SendMessage(m_hWND, WM_SYSCOMMAND, SC_MINIMIZE, NULL);
}

void SkinWindow::OnCloseClick(EventArgs& args)
{
	SendMessage(m_hWND, WM_CLOSE, NULL, NULL);
}
void SkinWindow::OnSelectChanged(TinyVisualOption* ps)
{
	TRACE("选中: %s\n", ps->GetText().CSTR());
}
void SkinWindow::OnButtonClick(EventArgs& args)
{
	TinyVisualRichText* txtURL = static_cast<TinyVisualRichText*>(GetDocument()->GetVisualByName("txtURL"));
	if (txtURL != NULL)
	{

	}
}
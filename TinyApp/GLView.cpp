#include "stdafx.h"
#include "GLView.h"


GLView::GLView()
{
}


GLView::~GLView()
{
}


void GLView::OnInitialize()
{
	TinyVisual* visual = this->GetDocument().GetVisualByName("sysclose");
	if (visual != NULL)
	{
		m_onCloseClick.Reset(new Delegate<void(TinyVisual*, EventArgs&)>(this, &GLView::OnCloseClick));
		visual->EVENT_CLICK += m_onCloseClick;
	}
}

void GLView::OnUninitialize()
{
	TinyVisual* visual = this->GetDocument().GetVisualByName("sysclose");
	if (visual != NULL)
	{
		visual->EVENT_CLICK -= m_onCloseClick;
	}
}

void GLView::OnCloseClick(TinyVisual*, EventArgs& args)
{
	SendMessage(Handle(), WM_CLOSE, NULL, NULL);
}
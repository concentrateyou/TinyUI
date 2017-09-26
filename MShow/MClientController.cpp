#include "stdafx.h"
#include "MClientController.h"

namespace MShow
{
	MClientController::MClientController(MClientWindow& view)
		:m_view(view)
	{

	}

	MClientController::~MClientController()
	{
	}

	BOOL MClientController::Initialize()
	{
		m_preview.Reset(new MPreviewController(m_view.m_previewView));
		if (!m_preview)
			return FALSE;
		if (!m_preview->Initialize())
		{
			LOG(ERROR) << "Preview Initialize Fail";
			return FALSE;
		}
		InitializeUI();
		return TRUE;
	}

	BOOL MClientController::Uninitialize()
	{
		if (m_preview != NULL)
		{
			m_preview->Close();
			m_preview.Reset(NULL);
		}
		return TRUE;
	}

	void MClientController::UpdateItem(SEARCH_ITEM& item)
	{
		m_item = item;
		TinyVisual* visual = m_view.GetDocument()->GetVisualByName("lblName");
		if (visual != NULL)
		{
			visual->SetText(m_item.szProgramName.c_str());
		}
		visual = m_view.GetDocument()->GetVisualByName("lblBeginTime");
		if (visual != NULL)
		{
			visual->SetText(m_item.szBeginTime.c_str());
		}
		visual = m_view.GetDocument()->GetVisualByName("lblEndTime");
		if (visual != NULL)
		{
			visual->SetText(m_item.szEndTime.c_str());
		}
		m_view.Invalidate();
	}

	void MClientController::InitializeUI()
	{
		TinyVisual* visual = m_view.GetDocument()->GetVisualByName("sysmin");
		if (visual != NULL)
		{
			m_onMinimumClick.Reset(new Delegate<void(TinyVisual*, EventArgs&)>(this, &MClientController::OnMinimumClick));
			visual->EVENT_CLICK += m_onMinimumClick;
		}
		visual = m_view.GetDocument()->GetVisualByName("sysclose");
		if (visual != NULL)
		{
			m_onCloseClick.Reset(new Delegate<void(TinyVisual*, EventArgs&)>(this, &MClientController::OnCloseClick));
			visual->EVENT_CLICK += m_onCloseClick;
		}
		visual = m_view.GetDocument()->GetVisualByName("previewWND");
		if (visual != NULL && visual->IsKindOf(RUNTIME_CLASS(TinyVisualNative)))
		{
			TinyVisualNative* val = static_cast<TinyVisualNative*>(visual);
			val->SetWindow(&m_view.m_previewView);
		}
	}

	void MClientController::OnMinimumClick(TinyVisual*, EventArgs& args)
	{
		m_view.GetDocument()->ReleaseCapture();//±ÿ–Î Õ∑≈≤∂ªÒ
		SendMessage(m_view.Handle(), WM_SYSCOMMAND, SC_MINIMIZE, NULL);
	}

	void MClientController::OnCloseClick(TinyVisual*, EventArgs& args)
	{
		SendMessage(m_view.Handle(), WM_CLOSE, NULL, NULL);
	}
}
#include "stdafx.h"
#include "MShowController.h"
#include "MPreviewController.h"

namespace MShow
{
	MShowController::MShowController(MShowWindow& window)
		:m_window(window)
	{

	}

	MShowController::~MShowController()
	{
	}

	BOOL MShowController::Initialize()
	{
		m_preview.Reset(new MPreviewController(m_window.m_previewView));
		if (!m_preview)
			return FALSE;
		if (!m_preview->Initialize())
			return FALSE;
		m_preview->Open("rtmp://10.10.13.100/live/lb_aoteman_720p");
		return TRUE;
	}

	void MShowController::Uninitialize()
	{
		if (m_preview != NULL)
		{
			m_preview->Close();
			m_preview.Reset(NULL);
		}
	}

	MPreviewController* MShowController::GetPreviewController()
	{
		return m_preview;
	}
}

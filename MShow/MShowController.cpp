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
		if (!m_preview->Open("rtmp://10.10.13.98/live/lb_xijudianying_720p"))
			return FALSE;
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

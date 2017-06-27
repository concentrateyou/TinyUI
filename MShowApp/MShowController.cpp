#include "stdafx.h"
#include "MShowController.h"
#include "MPreviewController.h"
#include "MVideoController.h"

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
		m_preview = new MPreviewController(m_window.m_previewView);
		if (!m_preview)
			return FALSE;
		if (!m_preview->Initialize(TinySize(1280, 720)))
			return FALSE;
		for (UINT i = 0;i < 6;i++)
		{
			m_videos[i] = new MVideoController(m_window.m_videoViews[i]);
			if (!m_videos[i])
				return FALSE;
			if (!m_videos[i]->Initialize())
				return FALSE;
			m_window.m_volumeViews[i].EVENT_VOLUME += m_videos[i]->m_onVolume;
		}
		m_preview->Submit();
		return TRUE;
	}

	void MShowController::Uninitialize()
	{
		if (m_preview != NULL)
		{
			if (m_preview->IsValid())
			{
				m_preview->Close(INFINITE);
			}
			SAFE_DELETE(m_preview);
		}
		for (UINT i = 0;i < 6;i++)
		{
			m_window.m_volumeViews[i].EVENT_VOLUME -= m_videos[i]->m_onVolume;
			if (m_videos[i])
			{
				m_videos[i]->Close();
			}
			SAFE_DELETE(m_videos[i]);
		}
	}

	MPreviewController* MShowController::GetPreviewController()
	{
		return m_preview;
	}

	MVideoController*	MShowController::GetVideoController(UINT i)
	{
		if (i < 0 || i > 5)
			return NULL;
		return m_videos[i];
	}
}

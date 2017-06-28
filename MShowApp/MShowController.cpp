#include "stdafx.h"
#include "MShowController.h"
#include "MPreviewController.h"
#include "MVideoController.h"
#include "MImageController.h"

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
		if (!m_preview->Initialize())
			return FALSE;

		m_play = new MPlayController(m_window.m_playView);
		if (!m_play)
			return FALSE;
		if (!m_play->Initialize())
			return FALSE;

		m_preview->SetPulgSize(TinySize(1280, 720));
		for (UINT i = 0;i < 6;i++)
		{
			m_videos[i] = new MVideoController(m_window.m_videoViews[i]);
			if (!m_videos[i])
				return FALSE;
			if (!m_videos[i]->Initialize())
				return FALSE;
			m_window.m_volumeViews[i].EVENT_VOLUME += m_videos[i]->m_onVolume;
			m_images[i] = new MImageController(m_window.m_imageViews[i]);
			if (!m_images[i])
				return FALSE;
			if (!m_images[i]->Initialize())
				return FALSE;
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
		if (m_play != NULL)
		{
			m_play->Close();
			SAFE_DELETE(m_play);
		}
		for (UINT i = 0;i < 6;i++)
		{
			m_window.m_volumeViews[i].EVENT_VOLUME -= m_videos[i]->m_onVolume;
			if (m_videos[i])
			{
				m_videos[i]->Close();
			}
			SAFE_DELETE(m_videos[i]);
			if (m_images[i])
			{
				m_images[i]->Close();
			}
			SAFE_DELETE(m_images[i]);
		}
	}

	MPreviewController* MShowController::GetPreviewController()
	{
		return m_preview;
	}

	MPlayController*	MShowController::GetPlayController()
	{
		return m_play;
	}

	MVideoController*	MShowController::GetVideoController(UINT i)
	{
		if (i < 0 || i > 5)
			return NULL;
		return m_videos[i];
	}

	MImageController* MShowController::GetImageController(UINT i)
	{
		if (i < 0 || i > 5)
			return NULL;
		return m_images[i];
	}
}

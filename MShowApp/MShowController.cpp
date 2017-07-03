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
		m_onPusherClick.Reset(new Delegate<void(void*, INT)>(this, &MShowController::OnPusher));
		m_window.m_pusher.EVENT_CLICK += m_onPusherClick;
		m_onToggleClick.Reset(new Delegate<void(void*, INT)>(this, &MShowController::OnToggle));
		m_window.m_toggle.EVENT_CLICK += m_onToggleClick;
		m_preview = new MPreviewController(m_window.m_previewView);
		if (!m_preview)
			return FALSE;
		if (!m_preview->Initialize())
			return FALSE;
		if (!m_encoder.Initialize(m_window.m_shadowView))
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
		m_window.m_pusher.EVENT_CLICK -= m_onPusherClick;
		m_window.m_toggle.EVENT_CLICK -= m_onToggleClick;
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
		m_encoder.Close();
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

	MRTMPEncoder& MShowController::GetEncoder()
	{
		return m_encoder;
	}

	MRTMPPusher& MShowController::GetPusher()
	{
		return m_pusher;
	}

	void MShowController::OnPusher(void*, INT)
	{
		MVideoController* pCTRL = GetVideoController(0);
		if (pCTRL != NULL && m_preview != NULL)
		{
			if (m_pusher.IsValid())
				m_pusher.Close(INFINITE);
			if (m_pusher.Connect())
				m_pusher.Submit();
			m_encoder.SetAudioConfig(*pCTRL->GetFormat(), 128);
			m_encoder.SetVideoConfig(m_preview->GetPulgSize(), 25, 1000);
			m_encoder.Close();
			m_encoder.Open();
		}
	}

	void MShowController::OnToggle(void*, INT)
	{

	}
}

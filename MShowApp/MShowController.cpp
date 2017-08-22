#include "stdafx.h"
#include "MShowController.h"
#include "MPreviewController.h"
#include "MShadowController.h"
#include "MVideoController.h"
#include "MImageController.h"
#include "MAudioController.h"

namespace MShow
{
	MShowController::MShowController(MShowWindow& window)
		:m_window(window),
		m_audio(m_pusher, m_clock),
		m_video(m_pusher, m_clock),
		m_pCTRL(NULL)
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
		m_preview.Reset(new MPreviewController(m_window.m_previewView));
		if (!m_preview)
			return FALSE;
		if (!m_preview->Initialize())
			return FALSE;
		m_preview->SetPulgSize(TinySize(1280, 720));
		m_shadow.Reset(new MShadowController(m_window.m_shadowView, m_clock));
		if (!m_shadow)
			return FALSE;
		if (!m_shadow->Initialize())
			return FALSE;
		m_shadow->SetPulgSize(TinySize(1280, 720));
		m_play.Reset(new MPlayController(m_window.m_playView));
		if (!m_play)
			return FALSE;
		if (!m_play->Initialize())
			return FALSE;
		m_window.m_playVolumeView.EVENT_VOLUME += m_play->m_onVolume;
		for (UINT i = 0;i < 6;i++)
		{
			m_videos[i].Reset(new MVideoController(m_window.m_videoViews[i]));
			if (!m_videos[i])
				return FALSE;
			if (!m_videos[i]->Initialize())
				return FALSE;
			m_window.m_volumeViews[i].EVENT_VOLUME += m_videos[i]->m_onVolume;
			m_images[i].Reset(new MImageController(m_window.m_imageViews[i]));
			if (!m_images[i])
				return FALSE;
			if (!m_images[i]->Initialize())
				return FALSE;
			m_audios[i].Reset(new MAudioController(m_window.m_audioViews[i]));
			if (!m_audios[i])
				return FALSE;
			if (!m_audios[i]->Initialize())
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
			if (m_preview->IsActive())
			{
				m_preview->Close(INFINITE);
			}
			m_preview.Reset(NULL);
		}
		if (m_play != NULL)
		{
			m_window.m_playVolumeView.EVENT_VOLUME -= m_play->m_onVolume;
			m_play->Close();
			m_play.Reset(NULL);
		}
		if (m_audio.IsActive())
		{
			m_audio.Close(INFINITE);
		}
		if (m_video.IsActive())
		{
			m_video.Close(INFINITE);
		}
		for (UINT i = 0;i < 6;i++)
		{
			m_window.m_volumeViews[i].EVENT_VOLUME -= m_videos[i]->m_onVolume;
			if (m_videos[i])
			{
				m_videos[i]->Close();
			}
			m_videos[i].Reset(NULL);
			if (m_images[i])
			{
				m_images[i]->Close();
			}
			m_images[i].Reset(NULL);
			if (m_audios[i])
			{
				m_audios[i]->Close();
			}
			m_audios[i].Reset(NULL);
		}
	}

	MPreviewController* MShowController::GetPreviewController()
	{
		return m_preview;
	}
	MShadowController*	MShowController::GetShadowController()
	{
		return m_shadow;
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

	MAudioEncodeTask& MShowController::GetAudioEncoder()
	{
		return m_audio;
	}

	MVideoEncodeTask& MShowController::GetVideoEncoder()
	{
		return m_video;
	}

	MRTMPPusher& MShowController::GetPusher()
	{
		return m_pusher;
	}
	void MShowController::SetCurrentCTRL(MVideoController* pCTRL)
	{
		m_pCTRL = pCTRL;
		m_audio.SetVideoController(pCTRL);
		if (m_preview != NULL)
		{
			m_preview->SetVideoController(pCTRL);
		}
	}
	MVideoController* MShowController::GetCurrentCTRL()
	{
		return m_pCTRL;
	}
	void MShowController::OnPusher(void*, INT)
	{
		MVideoController* pCTRL = GetVideoController(0);
		if (pCTRL != NULL && m_preview != NULL && m_shadow != NULL)
		{
			if (m_shadow->IsActive())
				m_shadow->Close(INFINITE);
			if (m_pusher.IsActive())
				m_pusher.Close(INFINITE);
			if (m_audio.IsActive())
				m_audio.Close(INFINITE);
			if (m_video.IsActive())
				m_video.Close(INFINITE);
			if (m_pusher.Connect())
				m_pusher.Submit();
			m_video.Submit(m_preview->GetPulgSize(), 25, 1000);
			m_audio.SetVideoController(pCTRL);
			m_audio.Submit(128);
			m_shadow->Submit();
			/*Sleep(100);
			if (m_play != NULL)
			{
				m_play->Open(m_pusher.GetURL().STR());
			}*/
		}
	}
	void MShowController::OnToggle(void*, INT)
	{

	}
}

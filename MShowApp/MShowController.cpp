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
		m_video(m_pusher, m_clock)
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
		m_previewCTRL.Reset(new MPreviewController(m_window.m_previewView));
		if (!m_previewCTRL)
			return FALSE;
		if (!m_previewCTRL->Initialize())
			return FALSE;
		m_previewCTRL->SetPulgSize(TinySize(1280, 720));
		m_shadowCTRL.Reset(new MShadowController(m_window.m_shadowView));
		if (!m_shadowCTRL)
			return FALSE;
		if (!m_shadowCTRL->Initialize())
			return FALSE;
		m_shadowCTRL->SetPulgSize(TinySize(1280, 720));
		m_play.Reset(new MPlayController(m_window.m_playView));
		if (!m_play)
			return FALSE;
		if (!m_play->Initialize())
			return FALSE;
		m_window.m_playVolumeView.EVENT_VOLUME += m_play->m_onVolume;	
		for (UINT i = 0;i < 6;i++)
		{
			m_videoCTRLS[i].Reset(new MVideoController(m_window.m_videoViews[i]));
			if (!m_videoCTRLS[i])
				return FALSE;
			if (!m_videoCTRLS[i]->Initialize())
				return FALSE;
			m_window.m_volumeViews[i].EVENT_VOLUME += m_videoCTRLS[i]->m_onVolume;
			m_imageCTRLS[i].Reset(new MImageController(m_window.m_imageViews[i]));
			if (!m_imageCTRLS[i])
				return FALSE;
			if (!m_imageCTRLS[i]->Initialize())
				return FALSE;
			m_audioCTRLS[i].Reset(new MAudioController(m_window.m_audioViews[i]));
			if (!m_audioCTRLS[i])
				return FALSE;
			if (!m_audioCTRLS[i]->Initialize())
				return FALSE;
		}
		m_previewCTRL->Submit();
		return TRUE;
	}

	void MShowController::Uninitialize()
	{
		m_window.m_pusher.EVENT_CLICK -= m_onPusherClick;
		m_window.m_toggle.EVENT_CLICK -= m_onToggleClick;
		if (m_previewCTRL != NULL)
		{
			if (m_previewCTRL->IsValid())
			{
				m_previewCTRL->Close(INFINITE);
			}
			m_previewCTRL.Reset(NULL);
		}
		if (m_play != NULL)
		{
			m_window.m_playVolumeView.EVENT_VOLUME -= m_play->m_onVolume;
			m_play->Close();
			m_play.Reset(NULL);
		}
		if (m_audio.IsValid())
		{
			m_audio.Close(INFINITE);
		}
		if (m_video.IsValid())
		{
			m_video.Close(INFINITE);
		}
		for (UINT i = 0;i < 6;i++)
		{
			m_window.m_volumeViews[i].EVENT_VOLUME -= m_videoCTRLS[i]->m_onVolume;
			if (m_videoCTRLS[i])
			{
				m_videoCTRLS[i]->Close();
			}
			m_videoCTRLS[i].Reset(NULL);
			if (m_imageCTRLS[i])
			{
				m_imageCTRLS[i]->Close();
			}
			m_imageCTRLS[i].Reset(NULL);
			if (m_audioCTRLS[i])
			{
				m_audioCTRLS[i]->Close();
			}
			m_audioCTRLS[i].Reset(NULL);
		}
	}

	MPreviewController* MShowController::GetPreviewController()
	{
		return m_previewCTRL;
	}
	MShadowController*	MShowController::GetShadowController()
	{
		return m_shadowCTRL;
	}
	MPlayController*	MShowController::GetPlayController()
	{
		return m_play;
	}

	MVideoController*	MShowController::GetVideoController(UINT i)
	{
		if (i < 0 || i > 5)
			return NULL;
		return m_videoCTRLS[i];
	}

	MImageController* MShowController::GetImageController(UINT i)
	{
		if (i < 0 || i > 5)
			return NULL;
		return m_imageCTRLS[i];
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

	void MShowController::OnPusher(void*, INT)
	{
		MVideoController* pCTRL = GetVideoController(0);
		if (pCTRL != NULL && m_previewCTRL != NULL && m_shadowCTRL != NULL)
		{
			if (m_shadowCTRL->IsValid())
			{
				m_shadowCTRL->Close(INFINITE);
			}
			if (m_pusher.IsValid())
			{
				m_pusher.Close(INFINITE);
			}
			if (m_audio.IsValid())
			{
				m_audio.Close(INFINITE);
			}
			if (m_video.IsValid())
			{
				m_video.Close(INFINITE);
			}
			if (m_pusher.Connect())
			{
				m_pusher.Submit();
			}
			m_video.Submit(m_previewCTRL->GetPulgSize(), 25, 1500);
			m_audio.SetVideoController(pCTRL);
			m_audio.Submit(128);
			m_shadowCTRL->Submit();
			if (m_play != NULL)
			{
				m_play->Open(m_pusher.GetURL().STR());
			}
		}
	}
	void MShowController::OnToggle(void*, INT)
	{

	}
}

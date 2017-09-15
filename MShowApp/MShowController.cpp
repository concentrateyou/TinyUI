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
		m_audioTask(m_pusher, m_clock),
		m_videoTask(m_pusher, m_clock),
		m_pVideoCTRL(NULL)
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
		m_previewCTRL->SetVideoFPS(25);
		m_previewCTRL->SetPulgSize(TinySize(1280, 720));
		m_shadowCTRL.Reset(new MShadowController(m_window.m_shadowView, m_clock));
		if (!m_shadowCTRL)
			return FALSE;
		if (!m_shadowCTRL->Initialize())
			return FALSE;
		m_shadowCTRL->SetPulgSize(TinySize(1280, 720));
		m_playCTRL.Reset(new MPlayController(m_window.m_playView));
		if (!m_playCTRL)
			return FALSE;
		if (!m_playCTRL->Initialize())
			return FALSE;
		m_window.m_playVolumeView.EVENT_VOLUME += m_playCTRL->m_onVolume;
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
		return m_previewCTRL->Submit();
	}

	void MShowController::Uninitialize()
	{
		m_window.m_pusher.EVENT_CLICK -= m_onPusherClick;
		m_window.m_toggle.EVENT_CLICK -= m_onToggleClick;
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
		if (m_previewCTRL != NULL && m_previewCTRL->IsActive())
		{
			m_previewCTRL->Close(INFINITE);
			m_previewCTRL.Reset(NULL);
		}
		if (m_playCTRL != NULL)
		{
			m_window.m_playVolumeView.EVENT_VOLUME -= m_playCTRL->m_onVolume;
			m_playCTRL->Close();
			m_playCTRL.Reset(NULL);
		}
		if (m_audioTask.IsActive())
		{
			m_audioTask.Close(INFINITE);
		}
		if (m_videoTask.IsActive())
		{
			m_videoTask.Close(INFINITE);
		}
		if (m_pusher.IsActive())
		{
			m_pusher.Close(INFINITE);
		}
		if (m_shadowCTRL != NULL)
		{
			m_shadowCTRL->Stop();
			m_shadowCTRL->Uninitialize();
			m_shadowCTRL.Reset(NULL);
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
		return m_playCTRL;
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
		return m_audioTask;
	}

	MVideoEncodeTask& MShowController::GetVideoEncoder()
	{
		return m_videoTask;
	}

	MRTMPPusher& MShowController::GetPusher()
	{
		return m_pusher;
	}
	void MShowController::SetCurrentCTRL(MVideoController* pCTRL)
	{
		m_pVideoCTRL = pCTRL;
		m_audioTask.SetVideoController(pCTRL);
	}
	MVideoController* MShowController::GetCurrentCTRL()
	{
		return m_pVideoCTRL;
	}
	void MShowController::OnPusher(void*, INT)
	{
		MVideoController* pCTRL = GetVideoController(0);
		if (pCTRL != NULL && m_previewCTRL != NULL && m_shadowCTRL != NULL)
		{
			m_shadowCTRL->Stop();
			if (m_pusher.IsActive())
				m_pusher.Close(INFINITE);
			if (m_audioTask.IsActive())
				m_audioTask.Close(INFINITE);
			if (m_videoTask.IsActive())
				m_videoTask.Close(INFINITE);
			if (m_pusher.Connect())
				m_pusher.Submit();
			m_videoTask.Submit(m_previewCTRL->GetPulgSize(), 25, 1000);
			m_audioTask.SetVideoController(pCTRL);
			m_audioTask.Submit(128);
			m_shadowCTRL->Start();
		}
	}
	void MShowController::OnToggle(void*, INT)
	{

	}
}

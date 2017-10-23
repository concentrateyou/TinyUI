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
		m_pVideoCTRL(NULL),
		m_bPushing(FALSE)
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
		m_previewCTRL.Reset(new MPreviewController(m_window.m_previewView, m_clock));
		if (!m_previewCTRL)
			return FALSE;
		if (!m_previewCTRL->Initialize())
			return FALSE;
		m_previewCTRL->SetVideoFPS(25);
		m_previewCTRL->SetPulgSize(TinySize(1280, 720));
		for (UINT i = 0;i < 6;i++)
		{
			m_videos[i].Reset(new MVideoController(m_window.m_videoViews[i], i));
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
		if (m_previewCTRL != NULL)
		{
			m_previewCTRL->Close(INFINITE);
			m_previewCTRL.Reset(NULL);
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
	}

	MPreviewController* MShowController::GetPreviewController()
	{
		return m_previewCTRL;
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
		ASSERT(m_previewCTRL);
		m_previewCTRL->Enter();
		m_pVideoCTRL = pCTRL;
		m_audioTask.SetVideoController(m_pVideoCTRL);
		m_previewCTRL->Leave();
	}
	MVideoController* MShowController::GetCurrentCTRL()
	{
		return m_pVideoCTRL;
	}
	BOOL MShowController::IsPushing() const
	{
		return m_bPushing;
	}
	void MShowController::OnPusher(void*, INT)
	{
		ASSERT(m_previewCTRL);

		if (m_bPushing)
		{
			m_bPushing = FALSE;
			if (m_pusher.IsActive())
			{
				m_pusher.Close(INFINITE);
			}
			if (m_audioTask.IsActive())
			{
				m_audioTask.Close(INFINITE);
			}
			if (m_videoTask.IsActive())
			{
				m_videoTask.Close(INFINITE);
			}
		}
		else
		{
			if (m_pusher.Connect())
			{
				m_pusher.Submit();
			}
			m_videoTask.Submit(m_previewCTRL->GetPulgSize(), 25, 1000);
			m_audioTask.Submit(128);
			m_bPushing = TRUE;
		}
	}
	void MShowController::OnToggle(void*, INT)
	{

	}
}

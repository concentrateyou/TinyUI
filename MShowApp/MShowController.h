#pragma once
#include "MShowWindow.h"
#include "MAudioEncodeTask.h"
#include "MVideoEncodeTask.h"
#include "MRTMPPusher.h"

namespace MShow
{
	class MVideoController;
	class MImageController;
	class MAudioController;
	class MPreviewController;
	class MPlayController;
	/// <summary>
	/// Ö÷¿ØÖÆÆ÷
	/// </summary>
	class MShowController
	{
		DISALLOW_COPY_AND_ASSIGN(MShowController)
	public:
		MShowController(MShowWindow& window);
		~MShowController();
		BOOL	Initialize();
		void	Uninitialize();
		MPreviewController* GetPreviewController();
		MAudioEncodeTask&	GetAudioEncoder();
		MVideoEncodeTask&	GetVideoEncoder();
		MRTMPPusher&		GetPusher();
		MVideoController*	GetCurrentCTRL();
		void				SetCurrentCTRL(MVideoController* pCTRL);
		BOOL				IsPushing() const;
	private:
		void OnPusher(void*, INT);
		void OnToggle(void*, INT);
	private:
		BOOL				m_bPushing;
		TinyLock			m_lock;
		MClock				m_clock;
		MShowWindow&		m_window;
		MRTMPPusher			m_pusher;
		MAudioEncodeTask	m_audioTask;
		MVideoEncodeTask	m_videoTask;
		MVideoController*	m_pVideoCTRL;
		TinyScopedPtr<MVideoController>		m_videos[6];
		TinyScopedPtr<MImageController>		m_images[6];
		TinyScopedPtr<MAudioController>		m_audios[6];
		TinyScopedPtr<MPreviewController>	m_previewCTRL;
		TinyScopedPtr<Delegate<void(void*, INT)>>	m_onToggleClick;
		TinyScopedPtr<Delegate<void(void*, INT)>>	m_onPusherClick;
	};
}



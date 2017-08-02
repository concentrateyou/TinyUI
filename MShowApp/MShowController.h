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
		MPlayController*	GetPlayController();
		MVideoController*	GetVideoController(UINT i);
		MImageController*	GetImageController(UINT i);
		MAudioEncodeTask&	GetAudioEncoder();
		MVideoEncodeTask&	GetVideoEncoder();
		MRTMPPusher&		GetPusher();
		void				SetBaseTime(LONG baseTime);
		LONG				GetBaseTime() const;
	private:
		void OnPusher(void*, INT);
		void OnToggle(void*, INT);
	private:
		TinyLock			m_lock;
		LONG				m_baseTime;
		MShowWindow&		m_window;
		MVideoController*	m_videos[6];
		MImageController*	m_images[6];
		MAudioController*	m_audios[6];
		MPreviewController*	m_preview;
		MPlayController*	m_play;
		MRTMPPusher			m_pusher;
		MAudioEncodeTask	m_audio;
		MVideoEncodeTask	m_video;
		TinyScopedPtr<Delegate<void(void*, INT)>>	m_onToggleClick;
		TinyScopedPtr<Delegate<void(void*, INT)>>	m_onPusherClick;
	};
}



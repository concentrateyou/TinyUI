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
		MShadowController*	GetShadowController();
		MPlayController*	GetPlayController();
		MVideoController*	GetVideoController(UINT i);
		MImageController*	GetImageController(UINT i);
		MAudioEncodeTask&	GetAudioEncoder();
		MVideoEncodeTask&	GetVideoEncoder();
		MRTMPPusher&		GetPusher();
	private:
		void OnPusher(void*, INT);
		void OnToggle(void*, INT);
	private:
		TinyLock			m_lock;
		MClock				m_clock;
		MShowWindow&		m_window;
		MRTMPPusher			m_pusher;
		MAudioEncodeTask	m_audio;
		MVideoEncodeTask	m_video;
		TinyScopedPtr<MVideoController>		m_videoCTRLS[6];
		TinyScopedPtr<MImageController>		m_imageCTRLS[6];
		TinyScopedPtr<MAudioController>		m_audioCTRLS[6];
		TinyScopedPtr<MPreviewController>	m_previewCTRL;
		TinyScopedPtr<MShadowController>	m_shadowCTRL;
		TinyScopedPtr<MPlayController>		m_play;
		TinyScopedPtr<Delegate<void(void*, INT)>>	m_onToggleClick;
		TinyScopedPtr<Delegate<void(void*, INT)>>	m_onPusherClick;
	};
}



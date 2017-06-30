#pragma once
#include "MShowWindow.h"
#include "MRTMPEncoder.h"
#include "MRTMPPusher.h"

namespace MShow
{
	class MVideoController;
	class MImageController;
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
		MRTMPEncoder&		GetEncoder();
		MRTMPPusher&		GetPusher();
	private:
		void OnPusher(void*, INT);
		void OnToggle(void*, INT);
	private:
		MShowWindow&		m_window;
		MVideoController*	m_videos[6];
		MImageController*	m_images[6];
		MPreviewController*	m_preview;
		MPlayController*	m_play;
		MRTMPEncoder		m_encoder;
		MRTMPPusher			m_pusher;
		TinyScopedPtr<Delegate<void(void*, INT)>>	m_onToggleClick;
		TinyScopedPtr<Delegate<void(void*, INT)>>	m_onPusherClick;
	};
}



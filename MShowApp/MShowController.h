#pragma once
#include "MShowWindow.h"

namespace MShow
{
	class MVideoController;
	class MImageController;
	class MPreviewController;
	class MPlayController;
	/// <summary>
	/// ��������
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
	private:
		MShowWindow&		m_window;
		MVideoController*	m_videos[6];
		MImageController*	m_images[6];
		MPreviewController*	m_preview;
		MPlayController*	m_play;
	};
}



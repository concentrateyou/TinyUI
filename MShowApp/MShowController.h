#pragma once
#include "MShowWindow.h"

namespace MShow
{
	class MVideoController;
	class MPreviewController;

	class MShowController
	{
		DISALLOW_COPY_AND_ASSIGN(MShowController)
	public:
		MShowController(MShowWindow& window);
		~MShowController();
		BOOL	Initialize();
		void	Uninitialize();
		MPreviewController* GetPreviewController();
		MVideoController*	GetVideoController(UINT i);
	private:
		MShowWindow&		m_window;
		MVideoController*	m_videos[6];
		MPreviewController*	m_preview;
	};
}



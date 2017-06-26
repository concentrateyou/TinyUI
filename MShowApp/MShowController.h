#pragma once
#include "MShowWindow.h"

namespace MShow
{
	class MShowController
	{
		DISALLOW_COPY_AND_ASSIGN(MShowController)
	public:
		MShowController(MShowWindow& window);
		~MShowController();
		BOOL	Initialize();
		void	Uninitialize();
	private:
		MShowWindow&		m_window;
		MVideoController*	m_videos[6];
	};
}



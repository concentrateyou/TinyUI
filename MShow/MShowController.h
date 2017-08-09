#pragma once
#include "MShowWindow.h"

namespace MShow
{
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
	private:
		void OnPusher(void*, INT);
		void OnToggle(void*, INT);
	private:
		TinyLock		m_lock;
		MShowWindow&	m_window;
		TinyScopedPtr<MPreviewController>	m_preview;
	};
}



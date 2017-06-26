#include "stdafx.h"
#include "MShowController.h"

namespace MShow
{
	MShowController::MShowController(MShowWindow& window)
		:m_window(window)
	{

	}

	MShowController::~MShowController()
	{
	}

	BOOL MShowController::Initialize()
	{
		for (UINT i = 0;i < 6;i++)
		{
			m_videos[i] = new MVideoController(m_window.m_videoViews[i]);
			if (!m_videos[i])
				return FALSE;
			if (!m_videos[i]->Initialize())
				return FALSE;
			m_window.m_volumeViews[i].EVENT_VOLUME += m_videos[i]->m_onVolume;
		}
		return TRUE;
	}

	void MShowController::Uninitialize()
	{
		for (UINT i = 0;i < 6;i++)
		{
			m_window.m_volumeViews[i].EVENT_VOLUME -= m_videos[i]->m_onVolume;
			if (m_videos[i])
			{
				m_videos[i]->Close();
			}
			SAFE_DELETE(m_videos[i]);
		}
	}
}

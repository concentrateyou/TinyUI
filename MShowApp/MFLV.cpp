#include "stdafx.h"
#include "MFLV.h"

namespace MShow
{
	MFLV::MFLV()
		:m_videoTask(m_task, m_clock),
		m_videoRenderTask(m_videoTask, m_clock),
		m_audioTask(m_task, m_clock),
		m_audioRenderTask(m_audioTask, m_clock)
	{

	}

	BOOL MFLV::Initialize(HWND hWND, LPCSTR pzURL)
	{
		if (!m_task.Initialize(pzURL))
			return FALSE;
		if (!m_videoRenderTask.Initialize(hWND))
			return FALSE;
		if (!m_audioRenderTask.Initialize(hWND))
			return FALSE;
		return TRUE;
	}

	BOOL MFLV::Submit()
	{
		m_close.CreateEvent();
		if (!m_task.Submit())
			return FALSE;
		if (!m_videoTask.Submit())
			return FALSE;
		if (!m_videoRenderTask.Submit())
			return FALSE;
		if (!m_audioTask.Submit())
			return FALSE;
		if (!m_audioRenderTask.Submit())
			return FALSE;
		return TRUE;
	}
	void MFLV::Close()
	{
		if (m_task.IsValid())
			m_task.Close(INFINITE);
		if (m_videoTask.IsValid())
			m_videoTask.Close(INFINITE);
		if (m_videoRenderTask.IsValid())
			m_videoRenderTask.Close(INFINITE);
		if (m_audioTask.IsValid())
			m_audioTask.Close(INFINITE);
		if (m_audioRenderTask.IsValid())
			m_audioRenderTask.Close(INFINITE);
	}

	MFLV::~MFLV()
	{

	}
}
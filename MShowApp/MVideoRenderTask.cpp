#include "stdafx.h"
#include "MVideoRenderTask.h"

namespace MShow
{
	MVideoRenderTask::MVideoRenderTask(MVideoTask& task, MClock& clock, TinyUI::Callback<void(BYTE*, LONG)>&& callback)
		:m_task(task),
		m_clock(clock),
		m_callback(std::move(callback)),
		m_bClose(FALSE)
	{
	}

	MVideoRenderTask::~MVideoRenderTask()
	{
	}

	BOOL MVideoRenderTask::Submit()
	{
		m_bClose = FALSE;
		return TinyTaskBase::Submit(BindCallback(&MVideoRenderTask::OnMessagePump, this));
	}

	BOOL MVideoRenderTask::Close(DWORD dwMS)
	{
		m_bClose = TRUE;
		return TinyTaskBase::Close(dwMS);
	}

	void MVideoRenderTask::OnMessagePump()
	{
		SampleTag sampleTag = { 0 };
		for (;;)
		{
			if (m_bClose)
				break;
			ZeroMemory(&sampleTag, sizeof(sampleTag));
			BOOL bRes = m_task.GetVideoQueue().Pop(sampleTag);
			if (!bRes || sampleTag.size <= 0)
			{
				Sleep(3);
				continue;
			}
			if (sampleTag.samplePTS == m_clock.GetBasePTS())
			{
				m_clock.SetBaseTime(timeGetTime());
			}
			while (m_clock.GetBasePTS() == -1);
			LONG ms = static_cast<LONG>(timeGetTime() - m_clock.GetBaseTime());
			INT delay = static_cast<INT>(sampleTag.samplePTS - ms);
			Sleep(delay < 0 ? 0 : delay);
			if (!m_callback.IsNull())
			{
				m_callback(sampleTag.bits, sampleTag.size);
			}
			SAFE_DELETE_ARRAY(sampleTag.bits);
		}
	}
}

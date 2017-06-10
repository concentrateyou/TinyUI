#pragma once
#include "MShowCommon.h"
#include "MReadTask.h"
#include "MVideoTask.h"
#include "MVideoRenderTask.h"
#include "MAudioTask.h"
#include "MAudioRenderTask.h"

namespace MShow
{
	class MShowRender
	{
		DISALLOW_COPY_AND_ASSIGN(MShowRender)
	public:
		MShowRender(MReadTask& task,MClock& clock);
		virtual ~MShowRender();
		BOOL Initialize(HWND hWND);
		BOOL Submit();
		BOOL Close();
	private:
		MReadTask&			m_task;
		MVideoTask			m_videoTask;
		MVideoRenderTask	m_videoRenderTask;
		MAudioTask			m_audioTask;
		MAudioRenderTask	m_audioRenderTask;
	};
}



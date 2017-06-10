#pragma once
#include "MShowCommon.h"
#include "MReadTask.h"
#include "MVideoTask.h"
#include "MVideoRenderTask.h"

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
		MVideoRenderTask	m_renderTask;
	};
}




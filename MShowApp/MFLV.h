#pragma once
#include "MShowCommon.h"
#include "MReadTask.h"
#include "MVideoTask.h"
#include "MVideoRenderTask.h"
#include "MAudioTask.h"
#include "MAudioRenderTask.h"

namespace MShow
{
	class MFLV 
	{
		DISALLOW_COPY_AND_ASSIGN(MFLV)
	public:
		MFLV();
		virtual ~MFLV();
		BOOL Initialize(HWND hWND, LPCSTR pzURL);
		BOOL Submit();
		void Close();
	private:
		TinyEvent			m_close;
		MClock				m_clock;
		MReadTask			m_task;
		MVideoTask			m_videoTask;
		MVideoRenderTask	m_videoRenderTask;
		MAudioTask			m_audioTask;
		MAudioRenderTask	m_audioRenderTask;
	};
}




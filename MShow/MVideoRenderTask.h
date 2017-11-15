#pragma once
#include "MShowCommon.h"
#include "MAudioTask.h"
#include "MVideoTask.h"
#include "DX9Graphics2D.h"
#include "DX9Image2D.h"
#include "DX9RenderView.h"
using namespace DXFramework;
using namespace TinyUI;

namespace MShow
{
	class MVideoTask;
	/// <summary>
	/// 视频播放线程
	/// </summary>
	class MVideoRenderTask : public TinyTaskBase
	{
		DISALLOW_COPY_AND_ASSIGN(MVideoRenderTask)
	public:
		MVideoRenderTask(MVideoTask& task, MClock& clock);
		virtual ~MVideoRenderTask();
		BOOL Initialize(HWND hWND);
		BOOL Submit();
		BOOL Close(DWORD dwMS) OVERRIDE;
	private:
		void OnMessagePump();
		BOOL OnCopy(BYTE* bits, LONG size);
	private:
		HWND				m_hWND;
		BOOL				m_bInitialize;
		BOOL				m_bBreak;
		TinySize			m_videoSize;
		MClock&				m_clock;
		MVideoTask&			m_task;
		DX9Graphics2D		m_graphics;
		DX9Image2D			m_image;
		TinyPerformanceTime m_timeQPC;
	};
}



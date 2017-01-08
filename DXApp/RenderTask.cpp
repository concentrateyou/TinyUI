#include "stdafx.h"
#include "RenderTask.h"
#include "DXWindow.h"

RenderTask::RenderTask()
{

}

RenderTask::~RenderTask()
{

}

BOOL RenderTask::Initialize(HWND hWND, INT cx, INT cy, DWORD dwFPS)
{
	m_dwFPS = dwFPS;
	if (!m_graphics.Initialize(hWND, cx, cy))
		return FALSE;

	return TRUE;
}

BYTE* RenderTask::GetPointer()
{
	return m_graphics.GetPointer();
}

BOOL RenderTask::Close(DWORD dwMs)
{
	m_close.SetEvent();
	return TinyTaskBase::Close(dwMs);
}

BOOL RenderTask::Submit()
{
	return TinyTaskBase::Submit(BindCallback(&RenderTask::OnMessagePump, this));
}

DWORD RenderTask::Render()
{
	m_timer.BeginTime();
	m_graphics.BeginScene();
	//TODO
	m_graphics.EndScene();
	m_timer.EndTime();
	return m_timer.GetMillisconds();
}

void RenderTask::OnRender(BYTE* bits, LONG size, FLOAT ts)
{
	EVENT_RENDER(bits, size, ts);
}

void RenderTask::OnExit()
{

}
void RenderTask::OnMessagePump()
{
	DWORD dwTime = 0;
	for (;;)
	{
		DWORD s = 1000 / m_dwFPS;
		s = dwTime > s ? 0 : s - dwTime;
		if (m_close.Lock(s))
		{
			OnExit();
			break;
		}
		dwTime = this->Render();
	}
}

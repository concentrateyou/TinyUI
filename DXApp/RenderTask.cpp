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
	if (!m_graphics.Initialize(hWND, TinySize(cx, cy)))
		return FALSE;
	return TRUE;
}
DX11Graphics2D*	 RenderTask::GetGraphics()
{
	return &m_graphics;
}

BOOL RenderTask::Close(DWORD dwMs)
{
	m_close.SetEvent();
	return TinyTaskBase::Close(dwMs);
}

BOOL RenderTask::Submit()
{
	m_close.CreateEvent(FALSE, FALSE, GenerateGUID().c_str(), NULL);
	return TinyTaskBase::Submit(BindCallback(&RenderTask::OnMessagePump, this));
}

DWORD RenderTask::Render()
{
	m_timer.BeginTime();
	m_graphics.BeginScene();

	for (INT i = 0;i < m_scenes.GetSize();i++)
	{
		DX11Element* ps = m_scenes[i];
		if (ps->GetElementType() == IMAGE)
		{
			ps->SetPosition(TinyPoint(10, 10));
			ps->SetScale(TinySize(ps->GetSize().cx / 2, ps->GetSize().cy / 2));
			static_cast<DX11Image*>(ps)->Update(m_graphics.GetDX11());
			m_graphics.DrawImage(static_cast<DX11Image*>(ps));
		}
	}
	m_graphics.EndScene();
	m_timer.EndTime();
	return m_timer.GetMillisconds();
}

void RenderTask::OnRender(BYTE* bits, LONG size, FLOAT ts)
{
	EVENT_RENDER(bits, size, ts);
}
BOOL RenderTask::Add(DX11Element* element)
{
	TinyAutoLock lock(m_lock);
	if (m_scenes.Lookup(element) >= 0)
		return FALSE;
	return m_scenes.Add(element);
}
void RenderTask::Remove(DX11Element* element)
{
	TinyAutoLock lock(m_lock);
	m_scenes.Remove(element);
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
			break;
		}
		TinyAutoLock lock(m_lock);
		dwTime = this->Render();
	}
}

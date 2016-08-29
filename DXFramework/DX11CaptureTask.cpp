#include "stdafx.h"
#include "DX11CaptureTask.h"

namespace DXFramework
{
	DX11CaptureTask::DX11CaptureTask()
		:m_hTask(NULL),
		m_dx11(NULL)
	{

	}


	DX11CaptureTask::~DX11CaptureTask()
	{
	}
	BOOL DX11CaptureTask::Initialize(DX11* dx11)
	{
		m_dx11 = dx11;
		m_hTask = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)DX11CaptureTask::Loop, this, 0, 0);
		return m_hTask != NULL;
	}
	DX11* DX11CaptureTask::GetDX11() const
	{
		return m_dx11;
	}
	DX11CaptureSource*	DX11CaptureTask::GetSource()
	{
		return &m_capture;
	}
	DWORD DX11CaptureTask::Loop(LPVOID lpUnused)
	{
		DX11CaptureTask* task = reinterpret_cast<DX11CaptureTask*>(lpUnused);
		for (;;)
		{
			task->GetSource()->Tick(*task->GetDX11());
		}
		return FALSE;
	}
}
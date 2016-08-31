#include "stdafx.h"
#include "DXRenderTask.h"
#include "DXSystem.h"


DXRenderTask::DXRenderTask(DXSystem* pSys, TinyTaskPool* pWorks)
	:TinyTask(pWorks),
	m_pSys(pSys),
	m_bCancel(FALSE)
{

}


DXRenderTask::~DXRenderTask()
{
}

BOOL DXRenderTask::Submit()
{
	m_bCancel = FALSE;
	Closure s = BindCallback(&DXRenderTask::MessagePump, this);
	return TinyTask::Submit(s);
}

void DXRenderTask::Cancel()
{
	m_bCancel = TRUE;
}

void DXRenderTask::MessagePump()
{
	for (;;)
	{
		if (m_bCancel)
			break;
		if (m_pSys)
		{
			m_pSys->Render();
		}
		Sleep(65);
	}
}
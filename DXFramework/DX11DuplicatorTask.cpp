#include "stdafx.h"
#include "DX11DuplicatorTask.h"

namespace DXFramework
{
	DX11DuplicatorTask::DX11DuplicatorTask(DX11& dx11, DX11Image2D& image)
		:m_image(image),
		m_dx11(dx11)
	{
		m_close.CreateEvent(FALSE, FALSE, GenerateGUID().c_str(), NULL);
	}
	DX11DuplicatorTask::~DX11DuplicatorTask()
	{

	}
	BOOL DX11DuplicatorTask::Submit()
	{
		return TinyThread::Submit(BindCallback(&DX11DuplicatorTask::OnMessagePump, this));
	}
	BOOL DX11DuplicatorTask::Close(DWORD dwMS)
	{
		m_close.SetEvent();
		return TinyThread::Close(dwMS);
	}
	void DX11DuplicatorTask::OnMessagePump()
	{
		for (;;)
		{
			if (m_close.Lock(5))
			{
				break;
			}
		}
	}
}

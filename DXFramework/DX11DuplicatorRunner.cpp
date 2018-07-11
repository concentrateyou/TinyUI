#include "stdafx.h"
#include "DX11DuplicatorRunner.h"

namespace DXFramework
{
	/*DX11DuplicatorRunner::DX11DuplicatorRunner(DX11& dx11, DX11Image2D& image)
		:m_image(image),
		m_dx11(dx11)
	{
		m_close.CreateEvent(FALSE, FALSE, GenerateGUID().c_str(), NULL);
	}
	DX11DuplicatorRunner::~DX11DuplicatorRunner()
	{

	}
	BOOL DX11DuplicatorRunner::Submit()
	{
		return TinyThread::Submit(BindCallback(&DX11DuplicatorRunner::OnMessagePump, this));
	}
	BOOL DX11DuplicatorRunner::Close(DWORD dwMS)
	{
		m_close.SetEvent();
		return TinyThread::Close(dwMS);
	}
	void DX11DuplicatorRunner::OnMessagePump()
	{
		for (;;)
		{
			if (m_close.Lock(5))
			{
				break;
			}
		}
	}*/
}

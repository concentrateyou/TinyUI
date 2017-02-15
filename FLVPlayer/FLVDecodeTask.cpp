#include "stdafx.h"
#include "FLVDecodeTask.h"

namespace FLVPlayer
{
	FLVDecodeTask::FLVDecodeTask()
	{
	}

	FLVDecodeTask::~FLVDecodeTask()
	{
	}
	BOOL FLVDecodeTask::Submit()
	{
		m_close.CreateEvent();
		return TinyTaskBase::Submit(BindCallback(&FLVDecodeTask::OnMessagePump, this));
	}
	BOOL FLVDecodeTask::Close(DWORD dwMs)
	{

	}
	void FLVDecodeTask::OnMessagePump()
	{
		for (;;)
		{
			if (m_close.Lock(0))
			{

			}
		}
	}
}


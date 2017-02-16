#include "stdafx.h"
#include "FLVVideoTask.h"

namespace FLVPlayer
{
	FLVVideoTask::FLVVideoTask()
	{
	}


	FLVVideoTask::~FLVVideoTask()
	{
	}
	BOOL FLVVideoTask::Submit(const TinySize& src, const TinySize& dst, BYTE* bits, LONG size)
	{
		ASSERT(m_h264);
		m_h264->Initialize(src, dst, BindCallback(&FLVVideoTask::OnH264, this));
		if (m_h264->Open(bits, size))
		{
			m_close.CreateEvent();
			return TinyTaskBase::Submit(BindCallback(&FLVVideoTask::OnMessagePump, this));
		}
		return FALSE;
	}
	BOOL FLVVideoTask::Close(DWORD dwMs)
	{
		m_close.SetEvent();
		return TRUE;
	}
	void FLVVideoTask::OnMessagePump()
	{
		for (;;)
		{
			if (m_close.Lock(1))
				break;

		}
	}
	void FLVVideoTask::OnH264(BYTE* bits, LONG size, LPVOID ps)
	{

	}
}
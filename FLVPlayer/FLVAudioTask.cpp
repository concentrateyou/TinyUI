#include "stdafx.h"
#include "FLVAudioTask.h"

namespace FLVPlayer
{
	FLVAudioTask::FLVAudioTask()
	{
		m_aac.Reset(new AACDecode());
	}


	FLVAudioTask::~FLVAudioTask()
	{
	}
	BOOL FLVAudioTask::Submit(BYTE* bits, LONG size, WORD wBitsPerSample)
	{
		ASSERT(m_aac);
		m_aac->Initialize(BindCallback(&FLVAudioTask::OnAAC, this));
		if (m_aac->Open(bits, size, wBitsPerSample))
		{
			m_close.CreateEvent();
			return TinyTaskBase::Submit(BindCallback(&FLVAudioTask::OnMessagePump, this));
		}
		return FALSE;
	}
	BOOL FLVAudioTask::Close(DWORD dwMs)
	{
		if (m_close)
		{
			m_close.SetEvent();
		}
		return TRUE;
	}
	void FLVAudioTask::OnMessagePump()
	{
		for (;;)
		{
			if (m_close.Lock(1))
				break;

		}
	}
	void FLVAudioTask::OnAAC(BYTE* bits, LONG size, LPVOID ps)
	{
		if (bits == NULL && size == 0)
		{
			m_waveFMT = *reinterpret_cast<WAVEFORMATEX*>(ps);
		}
		else
		{

		}
	}
}

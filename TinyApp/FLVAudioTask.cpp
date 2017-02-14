#include "stdafx.h"
#include "FLVAudioTask.h"
#include "FLVTask.h"


FLVAudioTask::FLVAudioTask(FLVTask* parentTask, HWND hWND, WAVEFORMATEX* pFMT)
	:m_parentTask(parentTask)
{
	m_player.Initialize(hWND, pFMT);
}


FLVAudioTask::~FLVAudioTask()
{
	m_player.Close();
}

BOOL FLVAudioTask::Submit()
{
	m_close.CreateEvent();
	return TinyTaskBase::Submit(BindCallback(&FLVAudioTask::OnMessagePump, this));
}
BOOL FLVAudioTask::Close(DWORD dwMS)
{
	m_close.SetEvent();
	return TinyTaskBase::Close(dwMS);
}
void FLVAudioTask::OnMessagePump()
{
	for (;;)
	{
		if (m_bits == NULL)
		{
			m_bits.Reset(new BYTE[8192]);
		}
		if (m_parentTask->m_queue.Read(m_bits, 8192))
		{
			m_player.Play(m_bits, 8192);
		}
	}
}
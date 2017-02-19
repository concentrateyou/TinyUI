#include "stdafx.h"
#include "FLVAudioTask.h"
#include "FLVDecodeTask.h"

namespace FLVPlayer
{
	FLVAudioTask::FLVAudioTask(FLVDecodeTask* pTask, TinyLock& lock, HWND hWND)
		:m_pTask(pTask),
		m_lock(lock),
		m_hWND(hWND),
		m_currentPTS(0)
	{
		m_aac.Reset(new AACDecode());
		m_wait.CreateEvent();
	}


	FLVAudioTask::~FLVAudioTask()
	{
	}
	BOOL FLVAudioTask::Submit(BYTE* bits, LONG size, WORD wBitsPerSample)
	{
		ASSERT(m_aac);
		if (m_aac->Open(bits, size, wBitsPerSample))
		{
			m_player.Initialize(m_hWND, &m_aac->GetFormat());
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
	void FLVAudioTask::Push(const SampleTag& tag)
	{
		m_lock.Lock();
		if (m_queue.size() >= 8)
		{
			m_lock.Unlock();
			m_wait.Lock(INFINITE);
		}
		m_queue.push(tag);
		m_lock.Unlock();
	}
	void FLVAudioTask::OnMessagePump()
	{
		for (;;)
		{
			if (m_close.Lock(15))
				break;
			LONG pts = 0;
			if (OnProcessTag(pts))
			{
				LONG offset = 0;
				if (m_currentPTS != pts)
				{
					offset = pts - m_currentPTS - 20;
					m_currentPTS = pts;
					Sleep(offset < 0 ? 0 : offset);
				}
			}
		}
	}
	BOOL FLVAudioTask::OnProcessTag(LONG& pts)
	{
		TinyAutoLock lock(m_lock);
		if (m_queue.empty())
			return FALSE;
		m_timer.BeginTime();
		SampleTag tag = m_queue.front();
		m_queue.pop();
		if (m_queue.size() <= 3)
		{
			m_wait.SetEvent();
		}
		BYTE* bo = NULL;
		LONG  so = 0;
		if (m_aac->Decode(tag.bits, tag.size, bo, so))
		{
			this->OnRender(bo, so);
		}
		SAFE_DELETE_ARRAY(tag.bits);
		m_timer.EndTime();
		pts = tag.pts - m_timer.GetMillisconds();
		return TRUE;
	}
	void FLVAudioTask::OnRender(BYTE* bits, LONG size)
	{
		m_audios.Add(bits, size);
		DWORD dwAVG = size * 20;
		if (m_audios.GetSize() >= dwAVG)
		{
			m_player.Play(m_audios.GetPointer(), m_audios.GetSize());
			m_audios.Remove(0, m_audios.GetSize());
		}
	}
}

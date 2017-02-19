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
	}


	FLVAudioTask::~FLVAudioTask()
	{
	}
	BOOL FLVAudioTask::Submit(BYTE* bits, LONG size, WORD wBitsPerSample)
	{
		ASSERT(m_aac);
		if (m_aac->Open(bits, size, wBitsPerSample))
		{
			if (m_player.Initialize(m_hWND, &m_aac->GetFormat(), 8192 * 2))
			{
				m_event[0].CreateEvent();
				m_event[1].CreateEvent();
				DSBPOSITIONNOTIFY vals[2];
				vals[0].dwOffset = 8192 * 1 - 1;
				vals[0].hEventNotify = m_event[0];
				vals[1].dwOffset = 8192 * 2 - 1;
				vals[1].hEventNotify = m_event[1];
				if (m_player.SetNotifys(2, vals))
				{
					m_player.Play();
				}
			}
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
		m_queue.push(tag);
		m_lock.Unlock();
	}
	void FLVAudioTask::OnMessagePump()
	{
		for (;;)
		{
			HANDLE handles[2] = { m_event[0],m_event[1] };
			HRESULT hRes = WaitForMultipleObjects(2, handles, FALSE, INFINITE);
			if (hRes >= WAIT_OBJECT_0 && hRes <= (WAIT_OBJECT_0 + 1))
			{
				LONG pts = 0;
				OnProcessTag(pts);
			}
		}
	}
	BOOL FLVAudioTask::OnProcessTag(LONG& pts)
	{
		TinyAutoLock lock(m_lock);
		if (m_queue.empty())
			return FALSE;
		SampleTag tag = m_queue.front();
		m_queue.pop();
		BYTE* bo = NULL;
		LONG  so = 0;
		if (m_aac->Decode(tag.bits, tag.size, bo, so))
		{
			this->OnRender(bo, so);
		}
		else
		{
			BYTE data[8192];
			ZeroMemory(data, 8192);
			this->OnRender(data, 8192);
		}
		SAFE_DELETE_ARRAY(tag.bits);
		return TRUE;
	}
	void FLVAudioTask::OnRender(BYTE* bits, LONG size)
	{
		m_player.Fill(bits, size);
	}
}

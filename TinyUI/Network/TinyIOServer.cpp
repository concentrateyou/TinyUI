#include "../stdafx.h"
#include "TinyIOServer.h"
#include <process.h>

namespace TinyUI
{
	namespace Network
	{
		TinyIOTask::TinyIOTask(IO::TinyIOCP* ps)
			:m_pIOCP(ps)
		{

		}
		TinyIOTask::~TinyIOTask()
		{

		}
		BOOL TinyIOTask::Close(DWORD dwMs)
		{
			m_close.SetEvent();
			return TinyTaskBase::Close(dwMs);
		}
		BOOL TinyIOTask::Submit()
		{
			m_close.CreateEvent(FALSE, FALSE, NULL, NULL);
			Closure s = BindCallback(&TinyIOTask::OnMessagePump, this);
			return TinyTaskBase::Submit(s);
		}
		void TinyIOTask::OnMessagePump()
		{
			ASSERT(m_pIOCP);
			for (;;)
			{
				if (m_close.Lock(0))
				{
					break;
				}
				DWORD dwNumberOfBytesTransferred = 0;
				PULONG_PTR lpCompletionKey = NULL;
				LPOVERLAPPED lpIO = NULL;
				if (!GetQueuedCompletionStatus(m_pIOCP, &dwNumberOfBytesTransferred, lpCompletionKey, &lpIO, INFINITE))
				{
					continue;
				}

			}
		}
		void TinyIOTask::OnCompletionStatus(IO_CONTEXT* pIO, DWORD dwError)
		{

		}
		//////////////////////////////////////////////////////////////////////////
		TinyScopedIOTaskArray::TinyScopedIOTaskArray(DWORD dwCount, IO::TinyIOCP* ps)
			:m_myP(NULL),
			m_dwCount(dwCount),
			m_pIOCP(ps)
		{
			m_myP = operator new (dwCount * sizeof(TinyIOTask));
			TinyIOTask *s = reinterpret_cast<TinyIOTask*>(m_myP);
			for (DWORD i = 0; i < dwCount; i++)
			{
				new (&s[i]) TinyIOTask(ps);
			}
		}
		TinyScopedIOTaskArray::~TinyScopedIOTaskArray()
		{
			TinyIOTask *s = reinterpret_cast<TinyIOTask*>(m_myP);
			for (DWORD i = 0; i < m_dwCount; i++)
			{
				s[i].~TinyIOTask();
			}
			SAFE_DELETE(m_myP);
		}
		DWORD TinyScopedIOTaskArray::GetSize() const
		{
			return m_dwCount;
		}
		TinyScopedIOTaskArray::operator TinyIOTask*() const
		{
			TinyIOTask *s = reinterpret_cast<TinyIOTask*>(m_myP);
			return s;
		}
		const TinyIOTask* TinyScopedIOTaskArray::operator[](INT index) const
		{
			ASSERT(m_myP);
			TinyIOTask *s = reinterpret_cast<TinyIOTask*>(m_myP);
			return s + index;
		}
		TinyIOTask*	TinyScopedIOTaskArray::operator[](INT index)
		{
			ASSERT(m_myP);
			TinyIOTask *s = reinterpret_cast<TinyIOTask*>(m_myP);
			return s + index;
		}
		//////////////////////////////////////////////////////////////////////////
		TinyIOServer::TinyIOServer(DWORD dwConcurrency)
		{
			m_iocp.Reset(new IO::TinyIOCP(dwConcurrency));
			if (m_iocp)
			{
				m_tasks.Reset(new TinyScopedIOTaskArray(dwConcurrency, m_iocp));
			}
		}
		void TinyIOServer::Invoke()
		{
			for (INT i = 0;i < m_tasks->GetSize();i++)
			{
				TinyIOTask* task = m_tasks[i];
				task->Submit();
			}
		}
		void TinyIOServer::Close()
		{
			for (INT i = 0;i < m_tasks->GetSize();i++)
			{
				TinyIOTask* task = m_tasks[i];
				task->Close(INFINITE);
			}
		}
	}
}
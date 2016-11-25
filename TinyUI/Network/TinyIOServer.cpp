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
			for (;;)
			{
				if (m_close.Lock(0))
				{
					break;
				}

			}
		}
		//////////////////////////////////////////////////////////////////////////
		TinyScopedIOTaskArray::TinyScopedIOTaskArray(DWORD dwCount, IO::TinyIOCP* ps)
			:m_myPtr(NULL),
			m_dwCount(dwCount),
			m_pIOCP(ps)
		{
			m_myPtr = operator new (dwCount * sizeof(TinyIOTask));
			TinyIOTask *s = reinterpret_cast<TinyIOTask*>(m_myPtr);
			for (DWORD i = 0; i < dwCount; i++)
			{
				new (&s[i]) TinyIOTask(ps);
			}
		}
		TinyScopedIOTaskArray::~TinyScopedIOTaskArray()
		{
			TinyIOTask *s = reinterpret_cast<TinyIOTask*>(m_myPtr);
			for (DWORD i = 0; i < m_dwCount; i++)
			{
				s[i].~TinyIOTask();
			}
			SAFE_DELETE(m_myPtr);
		}
		//////////////////////////////////////////////////////////////////////////
		TinyIOServer::TinyIOServer(DWORD dwConcurrency)
		{
			m_iocp.Reset(new IO::TinyIOCP(dwConcurrency));
			if(m_iocp)
			{
				m_tasks.Reset(new TinyScopedIOTaskArray(dwConcurrency, m_iocp));
			}
		}

		void TinyIOServer::Run()
		{
			for (INT i = 0;i < ARRAYSIZE_UNSAFE(m_tasks);i++)
			{
				m_tasks[i].Submit()
			}
		}
	}
}
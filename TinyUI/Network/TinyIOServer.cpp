#include "../stdafx.h"
#include "../Common/TinyUtility.h"
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
			m_close.CreateEvent(FALSE, FALSE, GenerateGUID().c_str(), NULL);
			Closure s = BindCallback(&TinyIOTask::OnMessagePump, this);
			return TinyTaskBase::Submit(s);
		}
		void TinyIOTask::OnMessagePump()
		{
			ASSERT(m_pIOCP);
			ULONG_PTR completionKey = 1111;
			for (;;)
			{
				if (m_close.Lock(0))
				{
					break;
				}
				DWORD dwNumberOfBytesTransferred = 0;
				LPOVERLAPPED lpIO = NULL;
				if (!GetQueuedCompletionStatus(m_pIOCP->Handle(), &dwNumberOfBytesTransferred, &completionKey, &lpIO, INFINITE))
				{
					TRACE("GetQueuedCompletionStatus error\n");
					if (lpIO == NULL)
					{
						break;
					}
					continue;
				}
				TRACE("GetQueuedCompletionStatus success\n");
				IO_CONTEXT* s = static_cast<IO_CONTEXT*>(lpIO);
				switch (s->OP)
				{
				case OP_ACCEPT:
				{
					ACCEPT_IO_CONTEXT* aio = static_cast<ACCEPT_IO_CONTEXT*>(lpIO);
					DWORD dwError = 0;
					if (setsockopt(aio->socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, NULL, 0) == SOCKET_ERROR)
					{
						dwError = WSAGetLastError();
					}
					else
					{
						m_pIOCP->Register((HANDLE)aio->socket, 0);
					}
					aio->callback(dwError, dwNumberOfBytesTransferred, s);
				}
				break;
				case OP_RECV:
				{

				}
				break;
				case OP_SEND:
				{

				}
				break;
				}
			}
		}
		void TinyIOTask::OnCompletionStatus(IO_CONTEXT* pIO, DWORD dwError)
		{

		}
		//////////////////////////////////////////////////////////////////////////
		TinyScopedIOTasks::TinyScopedIOTasks(DWORD dwCount, IO::TinyIOCP* ps)
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
		TinyScopedIOTasks::~TinyScopedIOTasks()
		{
			TinyIOTask *s = reinterpret_cast<TinyIOTask*>(m_myP);
			for (DWORD i = 0; i < m_dwCount; i++)
			{
				s[i].~TinyIOTask();
			}
			SAFE_DELETE(m_myP);
		}
		DWORD TinyScopedIOTasks::GetSize() const
		{
			return m_dwCount;
		}
		const TinyIOTask* TinyScopedIOTasks::operator[](INT index) const
		{
			ASSERT(m_myP);
			TinyIOTask *s = reinterpret_cast<TinyIOTask*>(m_myP);
			return s + index;
		}
		TinyIOTask*	TinyScopedIOTasks::operator[](INT index)
		{
			ASSERT(m_myP);
			TinyIOTask *s = reinterpret_cast<TinyIOTask*>(m_myP);
			return s + index;
		}
		//////////////////////////////////////////////////////////////////////////
		TinyIOServer::TinyIOServer(DWORD dwConcurrency)
		{
			m_iocp = new IO::TinyIOCP(dwConcurrency);
			if (!m_iocp)
				throw exception("new TinyIOCP Fail");
			m_tasks.Reset(new TinyScopedIOTasks(dwConcurrency, m_iocp));
			if (!m_tasks)
				throw exception("new TinyScopedIOTasks Fail");
		}
		TinyIOServer::~TinyIOServer()
		{
			Close();
		}
		IO::TinyIOCP*	TinyIOServer::GetIOCP() const
		{
			return m_iocp;
		}
		void TinyIOServer::Invoke()
		{
			for (INT i = 0;i < m_tasks->GetSize();i++)
			{
				TinyIOTask* task = (*m_tasks)[i];
				task->Submit();
			}
		}
		void TinyIOServer::Close()
		{
			for (INT i = 0;i < m_tasks->GetSize();i++)
			{
				TinyIOTask* task = (*m_tasks)[i];
				task->Close(INFINITE);
			}
		}
	}
}
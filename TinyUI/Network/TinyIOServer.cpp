#include "../stdafx.h"
#include "../Common/TinyUtility.h"
#include "TinyIOServer.h"
#include "TinySocket.h"
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
			ASSERT(m_pIOCP);
			PER_IO_CONTEXT* context = new PER_IO_CONTEXT();
			ZeroMemory(context, sizeof(PER_IO_CONTEXT));
			context->OP = OP_QUIT;
			PostQueuedCompletionStatus(m_pIOCP->Handle(), 0, 0, NULL);
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
			DWORD		errorCode = 0;
			ULONG_PTR	completionKey = 0;
			for (;;)
			{
				if (m_close.Lock(0))
				{
					break;
				}
				DWORD dwNumberOfBytesTransferred = 0;
				LPOVERLAPPED lpOP = NULL;
				if (!GetQueuedCompletionStatus(m_pIOCP->Handle(), &dwNumberOfBytesTransferred, &completionKey, &lpOP, INFINITE))
				{
					TinyScopedPtr<PER_IO_CONTEXT> context(static_cast<PER_IO_CONTEXT*>(lpOP));
					if (context != NULL)
					{
						TinySocket* s = reinterpret_cast<TinySocket*>(context->Reserve);
						errorCode = GetErrorCode(s->Handle(), lpOP);
						if (!context->Complete.IsNull())
						{
							context->Complete(errorCode, context->Result);
						}
					}
					continue;
				}
				TinyScopedPtr<PER_IO_CONTEXT> context(static_cast<PER_IO_CONTEXT*>(lpOP));
				if (!context || context->OP == OP_QUIT)
				{
					break;
				}
				if (dwNumberOfBytesTransferred == 0 && context->OP > OP_ACCEPT && context->OP < OP_CONNECT)
				{
					break;
				}
				switch (context->OP)
				{
				case OP_ACCEPT:
				{
					SOCKET listen = static_cast<SOCKET>(completionKey);
					TinySocket* s = reinterpret_cast<TinySocket*>(context->Reserve);
					if (s != NULL)
					{
						if (setsockopt(s->Handle(), SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (CHAR*)&listen, sizeof(listen)) == SOCKET_ERROR)
						{
							errorCode = WSAGetLastError();
						}
						else
						{
							if (!m_pIOCP->Register((HANDLE)s->Handle(), 0))
							{
								errorCode = WSAGetLastError();
							}
						}
						if (!context->Complete.IsNull())
						{
							TinySocket::AcceptAsyncResult* result = static_cast<TinySocket::AcceptAsyncResult*>(context->Result.Ptr());
							result->AcceptSocket = s;
							result->AcceptSocket->m_connect = errorCode == 0;
							context->Complete(errorCode, context->Result);
						}
					}
				}
				break;
				case OP_RECV:
				{
					TinySocket* s = reinterpret_cast<TinySocket*>(context->Reserve);
					if (s != NULL && !context->Complete.IsNull())
					{
						TinySocket::StreamAsyncResult* result = static_cast<TinySocket::StreamAsyncResult*>(context->Result.Ptr());
						result->BytesTransferred = dwNumberOfBytesTransferred;
						context->Complete(errorCode, context->Result);
					}
				}
				break;
				case OP_RECVFROM:
				{
					TinySocket* s = reinterpret_cast<TinySocket*>(context->Reserve);
					if (s != NULL && !context->Complete.IsNull())
					{
						TinySocket::DatagramAsyncResult* result = static_cast<TinySocket::DatagramAsyncResult*>(context->Result.Ptr());
						result->BytesTransferred = dwNumberOfBytesTransferred;
						context->Complete(errorCode, context->Result);
					}
				}
				break;
				case OP_SEND:
				{
					TinySocket* s = reinterpret_cast<TinySocket*>(context->Reserve);
					if (s != NULL && !context->Complete.IsNull())
					{
						TinySocket::StreamAsyncResult* result = static_cast<TinySocket::StreamAsyncResult*>(context->Result.Ptr());
						result->BytesTransferred = dwNumberOfBytesTransferred;
						context->Complete(errorCode, context->Result);
					}
				}
				break;
				case OP_SENDTO:
				{
					TinySocket* s = reinterpret_cast<TinySocket*>(context->Reserve);
					if (s != NULL && !context->Complete.IsNull())
					{
						TinySocket::DatagramAsyncResult* result = static_cast<TinySocket::DatagramAsyncResult*>(context->Result.Ptr());
						result->BytesTransferred = dwNumberOfBytesTransferred;
						context->Complete(errorCode, context->Result);
					}
				}
				break;
				case OP_CONNECT:
				{
					TinySocket* s = reinterpret_cast<TinySocket*>(context->Reserve);
					if (s != NULL)
					{
						if (setsockopt(s->Handle(), SOL_SOCKET, SO_UPDATE_CONNECT_CONTEXT, NULL, 0) == SOCKET_ERROR)
						{
							errorCode = WSAGetLastError();
						}
						else
						{
							if (!m_pIOCP->Register((HANDLE)s->Handle(), 0))
							{
								errorCode = WSAGetLastError();
							}
						}
						s->m_connect = TRUE;
						if (!context->Complete.IsNull())
						{
							context->Complete(errorCode, context->Result);
						}
					}
				}
				break;
				case OP_DISCONNECT:
				{
					TinySocket* s = reinterpret_cast<TinySocket*>(context->Reserve);
					if (s != NULL)
					{
						s->m_connect = FALSE;
						if (!context->Complete.IsNull())
						{
							context->Complete(errorCode, context->Result);
						}
					}
				}
				break;
				}
			}
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
		void TinyIOServer::Run()
		{
			for (DWORD i = 0;i < m_tasks->GetSize();i++)
			{
				TinyIOTask* task = (*m_tasks)[i];
				task->Submit();
			}
		}
		void TinyIOServer::Close()
		{
			for (DWORD i = 0;i < m_tasks->GetSize();i++)
			{
				TinyIOTask* task = (*m_tasks)[i];
				task->Close(INFINITE);
			}
		}
	}
}
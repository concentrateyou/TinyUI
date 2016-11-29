#pragma once
#include "TinyNetwork.h"
#include "TinySocket.h"
#include "../IO/TinyTaskBase.h"

namespace TinyUI
{
	namespace Network
	{
#define OP_ACCEPT	0x01
#define OP_RECV		0x02
#define OP_RECVFROM	0x03
#define OP_SEND		0x04
#define OP_SENDTO	0x05

		class NO_VTABLE IO_CONTEXT : public OVERLAPPED
		{
		public:
			DWORD		OP;
			ULONG_PTR	CompletionKey;
			Callback<void(DWORD, DWORD, IO_CONTEXT*)> Callback;
		};

		class NO_VTABLE ACCEPT_IO_CONTEXT : public IO_CONTEXT
		{
		public:
			SOCKET ListenSocket;
			SOCKET AcceptSocket;
		};

		class TinyIOTask : public IO::TinyTaskBase
		{
			DISALLOW_COPY_AND_ASSIGN(TinyIOTask)
		public:
			explicit TinyIOTask(IO::TinyIOCP* ps);
			virtual	~TinyIOTask();
			BOOL Close(DWORD dwMs = INFINITE) OVERRIDE;
			BOOL Submit();
			virtual void OnCompletionStatus(IO_CONTEXT* pIO, DWORD dwError);
		private:
			void OnMessagePump();
		private:
			IO::TinyIOCP*	m_pIOCP;
			TinyUI::Closure	m_closure;
			TinyEvent		m_close;
		};

		class TinyScopedIOTasks
		{
			DISALLOW_COPY_AND_ASSIGN(TinyScopedIOTasks)
		public:
			TinyScopedIOTasks(DWORD dwCount, IO::TinyIOCP* ps);
			~TinyScopedIOTasks();
			DWORD	GetSize() const;
			const TinyIOTask*	operator[](INT index) const;
			TinyIOTask*			operator[](INT index);
		private:
			void*			m_myP;
			DWORD			m_dwCount;
			IO::TinyIOCP*	m_pIOCP;
		};

		class TinyIOServer
		{
			DISALLOW_COPY_AND_ASSIGN(TinyIOServer)
		public:
			explicit TinyIOServer(DWORD dwConcurrency);
			virtual ~TinyIOServer();
			IO::TinyIOCP* GetIOCP() const;
			virtual void Invoke();
			virtual void Close();
		protected:
			TinyScopedReferencePtr<IO::TinyIOCP>	m_iocp;
			TinyScopedPtr<TinyScopedIOTasks>		m_tasks;
		};
	}
}


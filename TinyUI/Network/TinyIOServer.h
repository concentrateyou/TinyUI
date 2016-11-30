#pragma once
#include "TinyNetwork.h"
#include "TinySocket.h"
#include "../IO/TinyTaskBase.h"

namespace TinyUI
{
	namespace Network
	{
#define OP_UNKNOW		0x00
#define OP_ACCEPT		0x01
#define OP_RECV			0x02
#define OP_RECVFROM		0x03
#define OP_SEND			0x04
#define OP_SENDTO		0x05
#define OP_CONNECT		0x06
#define OP_DISCONNECT	0x07

#define MAX_BUFFER_SIZE   4096

		class TinyIOTask : public IO::TinyTaskBase
		{
			DISALLOW_COPY_AND_ASSIGN(TinyIOTask)
		public:
			explicit TinyIOTask(IO::TinyIOCP* ps);
			virtual	~TinyIOTask();
			BOOL Close(DWORD dwMs = INFINITE) OVERRIDE;
			BOOL Submit();
		private:
			void OnMessagePump();
		private:
			IO::TinyIOCP*	m_pIOCP;
			TinyUI::Closure	m_closure;
			TinyEvent		m_close;
			WSABUF			m_recv;
			WSABUF			m_send;
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
			virtual void Run();
			virtual void Close();
		protected:
			TinyScopedReferencePtr<IO::TinyIOCP>	m_iocp;
			TinyScopedPtr<TinyScopedIOTasks>		m_tasks;
		};
	}
}


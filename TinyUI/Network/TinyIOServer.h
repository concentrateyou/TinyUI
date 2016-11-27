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
#define OP_SEND		0x03

		class NO_VTABLE IO_CONTEXT : public OVERLAPPED
		{
		public:
			DWORD	OP;
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

		class TinyScopedIOTaskArray
		{
			DISALLOW_COPY_AND_ASSIGN(TinyScopedIOTaskArray)
		public:
			TinyScopedIOTaskArray(DWORD dwCount, IO::TinyIOCP* ps);
			~TinyScopedIOTaskArray();
			DWORD	GetSize() const;
			operator TinyIOTask*() const;
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
			virtual void Invoke();
			virtual void Close();
			//static void WINAPI CompletionRoutineCallback(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags);
		protected:
			TinyScopedPtr<IO::TinyIOCP>				m_iocp;
			TinyScopedPtr<TinyScopedIOTaskArray>	m_tasks;
		};
	}
}


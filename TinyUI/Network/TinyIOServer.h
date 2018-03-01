#pragma once
#include "TinyNetwork.h"
#include "TinySocket.h"
#include "../IO/TinyTask.h"

namespace TinyUI
{
	namespace Network
	{
#define MAX_BUFFER_SIZE   4096

		class TinyIOTask : public IO::TinyTask
		{
			DISALLOW_COPY_AND_ASSIGN(TinyIOTask)
		public:
			explicit TinyIOTask(IO::TinyIOCP* ps);
			virtual	~TinyIOTask();
			BOOL Close(DWORD dwMs = INFINITE) OVERRIDE;
			BOOL Submit();
		private:
			void	OnMessagePump();
		private:
			BOOL			m_bBreak;
			IO::TinyIOCP*	m_pIOCP;
			TinyUI::Closure	m_closure;
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
			TinyIOServer(DWORD dwConcurrency);
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


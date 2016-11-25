#pragma once
#include "TinyNetwork.h"
#include "TinySocket.h"
#include "../IO/TinyTaskBase.h"

namespace TinyUI
{
	namespace Network
	{
		class Runable
		{
		public:
			Runable();
			virtual ~Runable();
			virtual void Run();
		};

		class TinyIOTask : public IO::TinyTaskBase
		{
		public:
			TinyIOTask(IO::TinyIOCP* ps);
			virtual	~TinyIOTask();
			BOOL Close(DWORD dwMs = INFINITE) OVERRIDE;
			BOOL Submit();
		private:
			void OnMessagePump();
		private:
			IO::TinyIOCP*	m_pIOCP;
			TinyUI::Closure	m_closure;
			TinyEvent		m_close;
		};

		class TinyScopedIOTaskArray
		{
		public:
			TinyScopedIOTaskArray(DWORD dwCount, IO::TinyIOCP* ps);
			~TinyScopedIOTaskArray();
		private:
			void*			m_myPtr;
			DWORD			m_dwCount;
			IO::TinyIOCP*	m_pIOCP;
		};

		class TinyIOServer : public Runable
		{
		public:
			TinyIOServer(DWORD dwConcurrency);
			virtual void Run() OVERRIDE;
		protected:
			TinyScopedPtr<IO::TinyIOCP>				m_iocp;
			TinyScopedPtr<TinyScopedIOTaskArray>	m_tasks;
		};
	}
}


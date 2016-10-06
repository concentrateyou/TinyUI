#pragma once
#include "../Common/TinyCollection.h"
#include "../Common/TinyCore.h"
#include "../Common/TinyCallback.h"
#include "../Common/TinyEvent.h"
#include <string>
#include <WinSock2.h>
#pragma comment(lib,"Ws2_32.lib")
using namespace std;

namespace TinyUI
{
	namespace IO
	{
		/// <summary>
		/// 线程池
		/// </summary>
		class TinyWin32TaskPool : public TinyReference < TinyWin32TaskPool >
		{
			DISALLOW_COPY_AND_ASSIGN(TinyWin32TaskPool)
		public:
			TinyWin32TaskPool();
			~TinyWin32TaskPool();
			BOOL		Initialize(DWORD dwMin, DWORD dwMax);
			PTP_WORK	SubmitTask(PVOID ps, PTP_WORK_CALLBACK cb);
			void		WaitTask(PTP_WORK ps, BOOL fCancelPendingCallbacks);
			void		CloseTask(PTP_WORK ps);
			void		CancelPending();
			void		Close();
		private:
			PTP_POOL			m_pPool;
			PTP_CLEANUP_GROUP   m_pCleanup;
			TP_CALLBACK_ENVIRON m_cbe;
		};
		/// <summary>
		/// Win32任务
		/// </summary>
		class TinyWin32Task
		{
			DISALLOW_COPY_AND_ASSIGN(TinyWin32Task)
		public:
			explicit TinyWin32Task(TinyWin32TaskPool* pWorks);
			virtual ~TinyWin32Task();
			BOOL Submit(Closure& callback);
			BOOL Close();
		private:
			static void NTAPI WorkCallback(PTP_CALLBACK_INSTANCE Instance, PVOID  Context, PTP_WORK  Work);
		protected:
			PTP_WORK				m_work;
			TinyWin32TaskPool*		m_pWorks;
			Closure					m_callback;
		};
	}
}


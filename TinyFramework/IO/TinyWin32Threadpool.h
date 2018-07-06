#pragma once
#include "../Common/TinyCollection.h"
#include "../Common/TinyCore.h"
#include "../Common/TinyCallback.h"
#include "../Common/TinyEvent.h"
#include <string>
#include <WinSock2.h>
#pragma comment(lib,"Ws2_32.lib")
using namespace std;

namespace TinyFramework
{
	namespace IO
	{
		/// <summary>
		/// �̳߳�
		/// </summary>
		class TinyWin32Threadpool : public TinyReference < TinyWin32Threadpool >
		{
			friend class TinyWin32Worker;
			friend class TinyWin32Timer;
			friend class TinyWin32Waiter;
			DISALLOW_COPY_AND_ASSIGN(TinyWin32Threadpool)
		public:
			TinyWin32Threadpool();
			~TinyWin32Threadpool();
			BOOL		Initialize(DWORD dwMin, DWORD dwMax);
			void		Cancel(BOOL fCancelPendingCallbacks = TRUE);
			void		Close();
		private:
			PTP_POOL			m_pPool;
			PTP_CLEANUP_GROUP   m_pCleanup;
			TP_CALLBACK_ENVIRON m_cbe;
		};
		/// <summary>
		/// Win32����
		/// </summary>
		class TinyWin32Worker : public TinyReference < TinyWin32Worker >
		{
			DISALLOW_COPY_AND_ASSIGN(TinyWin32Worker)
		public:
			TinyWin32Worker(TinyWin32Threadpool* pThreadpool = NULL);
			virtual ~TinyWin32Worker();
			BOOL Submit(Closure&& callback);
			BOOL Wait(BOOL fCancelPendingCallbacks);
			void Close();
		private:
			static void NTAPI WorkCallback(PTP_CALLBACK_INSTANCE Instance, PVOID  Context, PTP_WORK  Work);
		protected:
			PTP_WORK				m_work;
			PTP_CALLBACK_ENVIRON	m_cbe;
			Closure					m_callback;
		};
		/// <summary>
		/// Win32��ʱ����
		/// </summary>
		class TinyWin32Timer
		{
			DISALLOW_COPY_AND_ASSIGN(TinyWin32Timer)
		public:
			TinyWin32Timer(TinyWin32Threadpool* pTaskPool = NULL);
			virtual ~TinyWin32Timer();
			BOOL Submit(DWORD msDelay, DWORD msPeriod, Closure&& callback);
			BOOL Wait(BOOL fCancelPendingCallbacks);
			void Close();
		private:
			static void NTAPI TimerCallback(PTP_CALLBACK_INSTANCE Instance, PVOID  Context, PTP_TIMER Timer);
		protected:
			PTP_TIMER				m_timer;
			PTP_CALLBACK_ENVIRON	m_cbe;
			Closure					m_callback;
		};
		/// <summary>
		/// Win32�ȴ�����
		/// </summary>
		class TinyWin32Waiter
		{
			DISALLOW_COPY_AND_ASSIGN(TinyWin32Waiter)
		public:
			TinyWin32Waiter(TinyWin32Threadpool* pTaskPool = NULL);
			virtual ~TinyWin32Waiter();
			BOOL Submit(HANDLE handle, DWORD msDelay, Closure&& callback);
			void Close();
		private:
			static void NTAPI WaitCallback(PTP_CALLBACK_INSTANCE Instance, PVOID  Context, PTP_WAIT Wait, TP_WAIT_RESULT WaitResult);
		protected:
			PTP_WAIT				m_wait;
			PTP_CALLBACK_ENVIRON	m_cbe;
			Closure					m_callback;
		};
	}
}

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
		/// 线程池
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
		/// Win32工作
		/// </summary>
		class TinyWin32Worker : public TinyReference < TinyWin32Worker >
		{
		public:
			explicit TinyWin32Worker(TinyWin32Threadpool* pool);
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
		/// Win32定时任务
		/// </summary>
		class TinyWin32Timer : public TinyReference<TinyWin32Timer>
		{
		public:
			explicit TinyWin32Timer(TinyWin32Threadpool* pool);
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
		/// Win32等待任务
		/// </summary>
		class TinyWin32Waiter : public TinyReference<TinyWin32Waiter>
		{
			DISALLOW_COPY_AND_ASSIGN(TinyWin32Waiter)
		public:
			explicit TinyWin32Waiter(TinyWin32Threadpool* pool);
			virtual ~TinyWin32Waiter();
			BOOL Submit(HANDLE handle, DWORD msDelay, Closure&& callback);
			BOOL Wait(BOOL fCancelPendingCallbacks);
			void Close();
		private:
			static void NTAPI WaitCallback(PTP_CALLBACK_INSTANCE Instance, PVOID  Context, PTP_WAIT Wait, TP_WAIT_RESULT WaitResult);
		protected:
			PTP_WAIT				m_wait;
			PTP_CALLBACK_ENVIRON	m_cbe;
			HANDLE					m_event;
			Closure					m_callback;
		};
	}
}


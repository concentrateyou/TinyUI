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
			friend class TinyWin32Task;
			friend class TinyWin32Timer;
			friend class TinyWin32Waiter;
			DISALLOW_COPY_AND_ASSIGN(TinyWin32TaskPool)
		public:
			TinyWin32TaskPool();
			~TinyWin32TaskPool();
			BOOL		Initialize(DWORD dwMin, DWORD dwMax);
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
		class TinyWin32Task : public TinyReference < TinyWin32Task >
		{
			DISALLOW_COPY_AND_ASSIGN(TinyWin32Task)
		public:
			TinyWin32Task(TinyWin32TaskPool* pTaskPool = NULL);
			virtual ~TinyWin32Task();
			BOOL Submit(Closure&& callback);
			BOOL Wait(BOOL fCancelPendingCallbacks);
			BOOL Close();
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
		class TinyWin32Timer
		{
			DISALLOW_COPY_AND_ASSIGN(TinyWin32Timer)
		public:
			TinyWin32Timer(TinyWin32TaskPool* pTaskPool = NULL);
			virtual ~TinyWin32Timer();
			BOOL Submit(DWORD msDelay, DWORD msPeriod, Closure&& callback);
			BOOL Wait(BOOL fCancelPendingCallbacks);
			BOOL Close();
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
		class TinyWin32Waiter
		{
			DISALLOW_COPY_AND_ASSIGN(TinyWin32Waiter)
		public:
			TinyWin32Waiter(TinyWin32TaskPool* pTaskPool = NULL);
			virtual ~TinyWin32Waiter();
			BOOL Submit(HANDLE handle, DWORD msDelay, Closure&& callback);
			BOOL Close();
		private:
			static void NTAPI WaitCallback(PTP_CALLBACK_INSTANCE Instance, PVOID  Context, PTP_WAIT Wait, TP_WAIT_RESULT WaitResult);
		protected:
			PTP_WAIT				m_wait;
			PTP_CALLBACK_ENVIRON	m_cbe;
			Closure					m_callback;
		};
	}
}


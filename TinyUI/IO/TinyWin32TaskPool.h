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
			PTP_WORK	SubmitTask(PVOID pWork, PTP_WORK_CALLBACK cb);
			void		WaitTask(PTP_WORK pWork, BOOL fCancelPendingCallbacks);
			void		CloseTask(PTP_WORK pWork);
			PTP_TIMER	SubmitTimer(PVOID pv, PTP_TIMER_CALLBACK cb, DWORD msDelay, DWORD msPeriod);
			void		SetTimer(PTP_TIMER pTimer, DWORD msDelay, DWORD msPeriod);
			void		WaitTimer(PTP_TIMER pTimer, BOOL fCancelPendingCallbacks);
			void		CloseTimer(PTP_TIMER pTimer);
			PTP_WAIT	SubmitWait(PVOID pv, PTP_WAIT_CALLBACK cb, HANDLE handle, DWORD msDelay);
			void		SetWait(PTP_WAIT pWait, HANDLE handle, DWORD msDelay);
			void		Wait(PTP_WAIT pWait, BOOL fCancelPendingCallbacks);
			void		CloseWait(PTP_WAIT pWait);
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
			explicit TinyWin32Task(TinyWin32TaskPool* pTaskPool);
			virtual ~TinyWin32Task();
			BOOL Submit(Closure&& callback);
			BOOL Wait(BOOL fCancelPendingCallbacks);
			BOOL Close();
		private:
			static void NTAPI WorkCallback(PTP_CALLBACK_INSTANCE Instance, PVOID  Context, PTP_WORK  Work);
		protected:
			PTP_WORK				m_work;
			TinyWin32TaskPool*		m_pTaskPool;
			Closure					m_callback;
		};
		/// <summary>
		/// Win32定时任务
		/// </summary>
		class TinyWin32Timer
		{
			DISALLOW_COPY_AND_ASSIGN(TinyWin32Timer)
		public:
			explicit TinyWin32Timer(TinyWin32TaskPool* pTaskPool);
			virtual ~TinyWin32Timer();
			BOOL Submit(DWORD msDelay, DWORD msPeriod, Closure&& callback);
			BOOL Wait(BOOL fCancelPendingCallbacks);
			BOOL Close();
		private:
			static void NTAPI TimerCallback(PTP_CALLBACK_INSTANCE Instance, PVOID  Context, PTP_TIMER Timer);
		protected:
			PTP_TIMER				m_timer;
			TinyWin32TaskPool*		m_pTaskPool;
			Closure					m_callback;
		};
		/// <summary>
		/// Win32等待任务
		/// </summary>
		class TinyWin32Waiter
		{
			DISALLOW_COPY_AND_ASSIGN(TinyWin32Waiter)
		public:
			explicit TinyWin32Waiter(TinyWin32TaskPool* pTaskPool);
			virtual ~TinyWin32Waiter();
			BOOL Submit(HANDLE handle, DWORD dwDelay, Closure&& callback);
			BOOL Close();
		private:
			static void NTAPI WaitCallback(PTP_CALLBACK_INSTANCE Instance, PVOID  Context, PTP_WAIT Wait, TP_WAIT_RESULT WaitResult);
		protected:
			PTP_WAIT				m_wait;
			TinyWin32TaskPool*		m_pTaskPool;
			Closure					m_callback;
		};
	}
}


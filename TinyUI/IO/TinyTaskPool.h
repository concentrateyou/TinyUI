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
		class TinyTaskPool : public TinyReference < TinyTaskPool >
		{
			DISALLOW_COPY_AND_ASSIGN(TinyTaskPool);
		public:
			TinyTaskPool();
			~TinyTaskPool();
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
		class TinyTask
		{
			DISALLOW_COPY_AND_ASSIGN(TinyTask);
		public:
			explicit TinyTask(TinyTaskPool* pWorks);
			virtual ~TinyTask();
			BOOL Submit(Closure& callback);
			BOOL Close();
		private:
			static void NTAPI WorkCallback(PTP_CALLBACK_INSTANCE Instance, PVOID  Context, PTP_WORK  Work);
		protected:
			PTP_WORK				m_work;
			TinyTaskPool*			m_pWorks;
			Closure					m_callback;
		};
	}
}


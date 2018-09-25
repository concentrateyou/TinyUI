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
		class TinyThreadpool : public TinyReference < TinyThreadpool >
		{
			friend class TinyThreadpoolWorker;
			friend class TinyThreadpoolTimer;
			friend class TinyThreadpoolWaiter;
			DISALLOW_COPY_AND_ASSIGN(TinyThreadpool)
		public:
			TinyThreadpool();
			~TinyThreadpool();
			BOOL		Initialize(DWORD dwMin, DWORD dwMax);
			void		Cancel(BOOL fCancelPendingCallbacks = TRUE);
			void		Close();
		private:
			PTP_POOL			m_pPool;
			PTP_CLEANUP_GROUP   m_pCleanup;
			TP_CALLBACK_ENVIRON m_cbe;
		};
		/// <summary>
		/// 线程池工作
		/// </summary>
		class TinyThreadpoolWorker : public TinyReference < TinyThreadpoolWorker >
		{
		public:
			explicit TinyThreadpoolWorker(TinyThreadpool& pool);
			virtual ~TinyThreadpoolWorker();
			BOOL Submit(Closure&& callback);
			BOOL Wait(BOOL fCancelPendingCallbacks);
			void Close();
		private:
			static void NTAPI WorkCallback(PTP_CALLBACK_INSTANCE Instance, PVOID  Context, PTP_WORK  Work);
		protected:
			Closure				m_callback;
			PTP_WORK			m_work;
			TinyThreadpool&		m_pool;
		};
		/// <summary>
		/// 线程池定时任务
		/// </summary>
		class TinyThreadpoolTimer : public TinyReference<TinyThreadpoolTimer>
		{
		public:
			explicit TinyThreadpoolTimer(TinyThreadpool& pool);
			virtual ~TinyThreadpoolTimer();
			BOOL Submit(DWORD msDelay, DWORD msPeriod, Closure&& callback);
			BOOL Wait(BOOL fCancelPendingCallbacks);
			void Close();
		private:
			static void NTAPI TimerCallback(PTP_CALLBACK_INSTANCE Instance, PVOID  Context, PTP_TIMER Timer);
		protected:
			Closure				m_callback;
			PTP_TIMER			m_timer;
			TinyThreadpool&		m_pool;
		};
		/// <summary>
		/// 线程池等待任务
		/// </summary>
		class TinyThreadpoolWaiter : public TinyReference<TinyThreadpoolWaiter>
		{
			DISALLOW_COPY_AND_ASSIGN(TinyThreadpoolWaiter)
		public:
			explicit TinyThreadpoolWaiter(TinyThreadpool& pool);
			virtual ~TinyThreadpoolWaiter();
			BOOL Submit(HANDLE handle, DWORD delay, Closure&& callback);
			BOOL Wait(BOOL fCancelPendingCallbacks);
			void Close();
		private:
			static void NTAPI WaitCallback(PTP_CALLBACK_INSTANCE Instance, PVOID  Context, PTP_WAIT Wait, TP_WAIT_RESULT WaitResult);
		protected:
			Closure				m_callback;
			PTP_WAIT			m_wait;
			TinyThreadpool&		m_pool;
		};

		class TaskScheduler;
		class TinyTaskRunner;
		class TinyThreadpoolTaskRunner;

		struct TaskRunnerTraits
		{
			static void Destruct(const TinyTaskRunner* runner);
		};

		class TinyTask : public TinyReference<TinyTask>
		{
			friend class TinyTaskRunner;
		public:
			TinyTask(Closure&& callback, INT32 delay);
			virtual ~TinyTask();
			virtual BOOL Invoke() = 0;
			virtual void Close() = 0;
		public:
			void		operator()();
			INT32		delay() const;
		protected:
			INT32		m_delay;
			Closure		m_callback;
		};

		class TinyThreadpoolTask : public TinyTask
		{
			DISALLOW_COPY_AND_ASSIGN(TinyThreadpoolTask)
		public:
			TinyThreadpoolTask(Closure&& callback, INT32 delay, TinyThreadpool& pool);
			virtual ~TinyThreadpoolTask();
			BOOL	Invoke() OVERRIDE;
			void	Close() OVERRIDE;
		private:
			void	OnInvoke();
		private:
			HANDLE					m_handle;
			TinyThreadpoolWorker	m_worker;
			TinyThreadpoolWaiter	m_waiter;
		};

		class TinyTaskRunner : public TinyReference<TinyTaskRunner, TaskRunnerTraits>
		{
		public:
			virtual	TinyScopedReferencePtr<TinyTask> CreateTask(Closure&& callback, INT32 delay) = 0;
			virtual	BOOL PostTask(TinyTask* task) = 0;
			virtual	BOOL PostTask(Closure&& callback, INT32 delay) = 0;
		protected:
			friend struct TaskRunnerTraits;
			friend class  TinyReference<TinyTaskRunner, TaskRunnerTraits>;
			TinyTaskRunner();
			virtual ~TinyTaskRunner();
			virtual void OnDestruct() const;
		};

		class TinyThreadpoolTaskRunner : public TinyTaskRunner
		{
			friend class TaskScheduler;
		public:
			TinyScopedReferencePtr<TinyTask> CreateTask(Closure&& callback, INT32 delay) OVERRIDE;
			BOOL PostTask(TinyTask* task) OVERRIDE;
			BOOL PostTask(Closure&& callback, INT32 delay) OVERRIDE;
		protected:
			TinyThreadpoolTaskRunner(INT32 iMax = 12);
			virtual ~TinyThreadpoolTaskRunner();
		private:
			TinyThreadpool	m_pool;
		};

		class TaskScheduler
		{
			DISALLOW_COPY_AND_ASSIGN(TaskScheduler)
		public:
			TaskScheduler();
			~TaskScheduler();
			TinyScopedReferencePtr<TinyTaskRunner> CreateTaskRunner();
		};
	}
}


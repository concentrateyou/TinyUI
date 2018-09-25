#pragma once
#include "../Common/TinyCollection.h"
#include "../Common/TinyCore.h"
#include "../Common/TinyCallback.h"
#include "../Common/TinyEvent.h"
#include "../IO/TinyWin32Threadpool.h"
using namespace TinyFramework;

namespace TinyFramework
{
	namespace IO
	{
		class TinyTaskRunner : public TinyReference<TinyTaskRunner>
		{
		public:
			virtual BOOL PostTask(Closure&& task, INT delay) = 0;
		};

		class TinyWorkerPoolTaskRunner : public TinyTaskRunner
		{
		private:
			class TinyTask
			{
				friend class TinyWorkerPoolTaskRunner;
			private:
				TinyTask();
				~TinyTask();
				BOOL Execute();
				void OnTask();
			private:
				INT					m_delay;
				HANDLE				m_event;
				Closure				m_callback;
				TinyWin32Worker*	m_worker;
				TinyWin32Waiter*	m_waiter;
			};
		public:
			TinyWorkerPoolTaskRunner(INT iMax = 12);
			~TinyWorkerPoolTaskRunner();
		public:
			BOOL PostTask(Closure&& callback, INT delay) OVERRIDE;
		private:
			TinyWin32Threadpool	m_pool;
		};

		class TinyTaskManeger
		{
		public:
			class TinyTask : public TinyReference<TinyTask>
			{
				friend class TinyTaskManeger;
			public:
				TinyTask();
				~TinyTask();
				BOOL Execute();
				void OnTask();
				void Close();
			private:
				INT32									m_delay;
				HANDLE									m_event;
				Closure									m_callback;
				TinyScopedReferencePtr<TinyWin32Worker> m_worker;
				TinyScopedReferencePtr<TinyWin32Waiter> m_waiter;
			};
		public:
			TinyTaskManeger(INT iMax = 12);
			~TinyTaskManeger();
			TinyScopedReferencePtr<TinyTask> PostTask(Closure&& callback, INT delay);
		private:
			TinyWin32Threadpool		m_pool;
		};
	};
}


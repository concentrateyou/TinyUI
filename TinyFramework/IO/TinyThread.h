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
		/// 线程本地存储
		/// </summary>
		class TinyTLS
		{
			DISALLOW_COPY_AND_ASSIGN(TinyTLS)
		public:
			TinyTLS();
			~TinyTLS();
			void	SetValue(LPVOID ps);
			LPVOID	GetValue();
		private:
			DWORD m_dwTlsIndex;
		};
		/// <summary>
		/// 线程基类
		/// </summary>
		class TinyThread
		{
			DISALLOW_COPY_AND_ASSIGN(TinyThread)
		public:
			TinyThread();
			virtual ~TinyThread();
			HANDLE	Handle() const;
			DWORD	GetID() const;
			BOOL	IsActive() const;
			BOOL	SetPriority(DWORD dwPriority);
			DWORD	Suspend();
			DWORD   Resume();
			BOOL	Terminate(DWORD dwExit);
		public:
			virtual BOOL Submit(Closure&& callback);
			virtual BOOL Close(DWORD dwMS);
		private:
			static DWORD WINAPI Callback(LPVOID ps);
		protected:
			DWORD		m_dwID;
			HANDLE		m_handle;
			Closure		m_callback;
		};
		/// <summary>
		/// 简单定时任务
		/// </summary>
		class TinySimpleTaskTimer
		{
			DISALLOW_COPY_AND_ASSIGN(TinySimpleTaskTimer)
		public:
			TinySimpleTaskTimer();
			~TinySimpleTaskTimer();
			BOOL SetCallback(LONG delay, Closure&& callback);
			void Close();
		private:
			void OnMessagePump();
		public:
			BOOL				m_bBreak;
			LONG				m_delay;
			Closure				m_callback;
			TinyThread			m_runner;
		};
	};
}


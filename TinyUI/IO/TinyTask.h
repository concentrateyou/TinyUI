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
#define WM_MSGQUEUE_EXIT (WM_USER + 0x001)
		/// <summary>
		/// �̻߳���
		/// </summary>
		class TinyTask
		{
			DISALLOW_COPY_AND_ASSIGN(TinyTask)
		public:
			TinyTask();
			virtual ~TinyTask();
			HANDLE	Handle() const;
			DWORD	GetTaskID() const;
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
			DWORD		m_dwTaskID;
			HANDLE		m_hTask;
			Closure		m_callback;
		};
		/// <summary>
		/// ��ʱ��
		/// </summary>
		class TinyTaskTimer
		{
			DISALLOW_COPY_AND_ASSIGN(TinyTaskTimer)
		public:
			TinyTaskTimer();
			~TinyTaskTimer();
			BOOL SetCallback(LONG delay, Closure&& callback);
			void Close();
		private:
			void OnMessagePump();
		public:
			BOOL				m_bBreak;
			LONG				m_delay;
			Closure				m_callback;
			TinyTask			m_task;
		};
		/// <summary>
		/// ��Ϣ����
		/// </summary>
		class TinyMsgQueue
		{
			DISALLOW_COPY_AND_ASSIGN(TinyMsgQueue)
		public:
			TinyMsgQueue();
			~TinyMsgQueue();
			BOOL SetCallback(Callback<void(UINT, WPARAM, LPARAM)>&& callback);
			BOOL PostMsg(MSG& msg);
			BOOL Close();
		private:
			void OnMessagePump();
		private:
			TinyTask							m_task;
			Callback<void(UINT, WPARAM, LPARAM)>	m_callback;
		};
	};
}


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
		/// �̻߳���
		/// </summary>
		class TinyTaskBase
		{
		public:
			TinyTaskBase();
			virtual ~TinyTaskBase();
			BOOL	IsValid() const;
			virtual BOOL Submit(Closure&& callback);
			virtual BOOL Close(DWORD dwMs);
			DWORD	GetCurrentTime() const;
		private:
			static DWORD WINAPI Callback(LPVOID ps);
		protected:
			DWORD		m_dwTaskID;
			HANDLE		m_hTask;
			Closure		m_callback;
			static DWORD m_dwTime;
		};
	};
}


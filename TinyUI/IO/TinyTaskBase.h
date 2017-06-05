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
		/// 线程基类
		/// </summary>
		class TinyTaskBase
		{
		public:
			TinyTaskBase();
			virtual ~TinyTaskBase();
			HANDLE	Handle() const;
			BOOL	IsValid() const;
			BOOL	SetPriority(DWORD dwPriority);
			virtual BOOL Submit(Closure&& callback);
			virtual BOOL Close(DWORD dwMS);
		private:
			static DWORD WINAPI Callback(LPVOID ps);
		protected:
			DWORD		m_dwTaskID;
			HANDLE		m_hTask;
			Closure		m_callback;
		};
	};
}


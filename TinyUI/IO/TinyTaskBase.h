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
		class TinyTaskBase
		{
		public:
			TinyTaskBase();
			virtual ~TinyTaskBase();
			BOOL	IsValid() const;
			virtual BOOL Submit(Closure& callback);
			virtual BOOL Wait(DWORD dwMs);
			virtual void Quit() = 0;
		private:
			static DWORD WINAPI Callback(LPVOID ps);
		private:
			DWORD		m_dwTaskID;
			HANDLE		m_hTask;
			Closure		m_callback;
		};
	};
}


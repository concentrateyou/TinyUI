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
		class TinyTask
		{
		public:
			TinyTask();
			virtual ~TinyTask();
			BOOL Submit(Closure& callback);
			BOOL Wait(DWORD dwMs);
			BOOL IsValid() const;
		private:
			static DWORD WINAPI Callback(LPVOID ps);
		private:
			DWORD		m_dwTaskID;
			HANDLE		m_hTask;
			Closure		m_callback;
		};
	};
}


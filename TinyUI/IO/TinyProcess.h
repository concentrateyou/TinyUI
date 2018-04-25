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
		/// 进程基类
		/// </summary>
		class TinyProcess
		{
		public:
			explicit TinyProcess(HANDLE handle = NULL);
			TinyProcess(TinyProcess&& other);
			TinyProcess& operator=(TinyProcess&& other);
			virtual ~TinyProcess();
			operator HANDLE() const;
			void SetInput(HANDLE handle);
			void SetOutput(HANDLE handle);
			void SetError(HANDLE handle);
			BOOL IsEmpty() const;
			DWORD GetPID() const;
			INT GetPriority() const;
			BOOL Create(const string& program, string& command);
			BOOL Open(DWORD dwPID);
			BOOL Close(DWORD dwMS = INFINITE);
			BOOL Terminate(UINT exitCode);
			BOOL WaitForTerminate(UINT exitCode, DWORD dwMS = 60 * 1000);
			TinyProcess Duplicate(HANDLE handle);
			static TinyProcess Current();
		private:
			HANDLE	m_hProcess;
			HANDLE	m_handles[3];
		};
	};
}


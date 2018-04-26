#pragma once
#include "../Common/TinyCollection.h"
#include "../Common/TinyCore.h"
#include "../Common/TinyCallback.h"
#include "../Common/TinyEvent.h"
#include <string>
#include <vector>
#include <WinSock2.h>
#pragma comment(lib,"Ws2_32.lib")
using namespace std;

namespace TinyUI
{
	namespace IO
	{
		/// <summary>
		/// 進程管道
		/// </summary>
		class TinyProcessPipe
		{
			DISALLOW_COPY_AND_ASSIGN(TinyProcessPipe)
		public:
			TinyProcessPipe();
			~TinyProcessPipe();
			BOOL Create();
			HANDLE GetInput() const;
			HANDLE GetOutput() const;
			DWORD Read(void* bits, DWORD size);
			DWORD Write(const void* bits, DWORD size);
			void CloseInput();
			void CloseOutput();
		private:
			HANDLE	m_hRIO;
			HANDLE	m_hWIO;
		};
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
			BOOL IsEmpty() const;
			DWORD GetPID() const;
			INT GetPriority() const;
			void SetInput(HANDLE handle);
			void SetOutput(HANDLE handle);
			void SetError(HANDLE handle);
			BOOL Create(const string& app, const vector<string>& args);
			BOOL Open(DWORD dwPID);
			BOOL Wait(DWORD dwMS, DWORD& dwExitCode);//等待進程退出
			BOOL Terminate(DWORD dwMS = 60 * 1000);
			TinyProcess Duplicate(HANDLE handle);
			static TinyProcess Current();
		private:
			HANDLE	m_hProcess;
			HANDLE	m_handles[3];
		};
	};
}


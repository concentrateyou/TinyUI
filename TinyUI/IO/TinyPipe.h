#pragma once
#include "../Common/TinyCollection.h"
#include "../Common/TinyCore.h"
#include "../Common/TinyCallback.h"
#include "../Common/TinyEvent.h"
#include "../Network/TinyNetwork.h"
#include <string>
#include <algorithm>
using namespace std;

namespace TinyUI
{
	namespace IO
	{
		class  PIPE_IO_CONTEXT : public OVERLAPPED
		{
		public:
			PIPE_IO_CONTEXT();
			~PIPE_IO_CONTEXT();
			void Reset();
		public:
			LONG_PTR							Context;
			Network::CompleteCallback			Complete;
			TinyScopedReferencePtr<Network::AsyncResult>	Result;
		};
		/// <summary>
		/// 管道
		/// </summary>
		class TinyPipe
		{
			DISALLOW_COPY_AND_ASSIGN(TinyPipe)
		public:
			TinyPipe();
			virtual ~TinyPipe();
			virtual BOOL Open(LPCSTR lpszPipename) = 0;
			virtual BOOL Connect() = 0;
		public:
			virtual void Close();
		protected:
			HANDLE	m_handles[2];//读写句柄
		};
		/// <summary>
		/// 客户端管道
		/// https://msdn.microsoft.com/en-us/library/windows/desktop/aa365592(v=vs.85).aspx
		/// </summary>
		class TinyPipeClient :public TinyPipe
		{
			DISALLOW_COPY_AND_ASSIGN(TinyPipeClient)
		public:
			TinyPipeClient();
			virtual ~TinyPipeClient();
		public:
			BOOL Open(LPCSTR lpszPipename) OVERRIDE;
			BOOL Connect() OVERRIDE;
			void Close() OVERRIDE;
		public:
			BOOL IsEmpty() const;
		private:
			string			m_szPipename;
			HANDLE			m_hFile;
			PIPE_IO_CONTEXT	m_context;
		};
	};
}


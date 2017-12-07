#pragma once
#include "TinySocket.h"

namespace TinyUI
{
	namespace Network
	{
		/// <summary>
		/// TCPÌ×½Ó×Ö
		/// </summary>
		class TinyTCPSocket : public TinyHandleSOCKET
		{
			DISALLOW_COPY_AND_ASSIGN(TinyTCPSocket)
		private:
			class Context
			{
			public:
				INT					m_op;
				TinyTCPSocket*		m_this;
				CompletionCallback	m_callback;
			};
		public:
			TinyTCPSocket();
			virtual ~TinyTCPSocket();
		public:
			BOOL Open(AddressFamily addressFamily);
			BOOL Close();
			BOOL IsEmpty() const;
			BOOL ConnectAsync(const IPEndPoint& endpoint, DWORD dwMS, CompletionCallback&& callback);
			BOOL ReceiveAsync(CHAR* data, DWORD dwSize, DWORD dwFlags, CompletionCallback&& callback);
		public:
			virtual void OnError(INT iError);
		private:
			BOOL SetAsyncEventSelect(INT op, INT bits, DWORD dwMS, HANDLE hEvent, CompletionCallback&& callback);
			void UnsetAsyncEventSelect(HANDLE hEvent);
			void ConnectCallback(Context* context, BOOL time);
			void ReceiveCallback(Context* context);
			static void CALLBACK WaitOrTimerCallback(void* ps, BOOLEAN time);
		private:
			HANDLE		m_handle;
			WSABUF		m_readBuffer;
			WSABUF		m_writeBuffer;
			OVERLAPPED  m_readIO;
			OVERLAPPED	m_writeIO;
		};
	}
}


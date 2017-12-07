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
			DISALLOW_COPY_AND_ASSIGN(TinyTCPSocket);
			class Context
			{
			public:
				INT					m_currentFD;
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
			BOOL Connect(const IPEndPoint& endpoint, DWORD dwMS, CompletionCallback&& callback);
		public:
			virtual void OnError(INT iError);
		private:
			BOOL SetAsyncEventSelect(INT iFD, DWORD dwMS, CompletionCallback&& callback);
			void UnsetAsyncEventSelect();
			void ConnectCallback(Context* ps, BOOL time);
			static void CALLBACK WaitOrTimerCallback(void* ps, BOOLEAN time);
		private:
			HANDLE		m_handle;
			HANDLE		m_event;

		};
	}
}


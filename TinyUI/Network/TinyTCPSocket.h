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
			static void CALLBACK WaitOrTimeCallback(void* ps, BOOLEAN time);
		private:
			HANDLE		m_handle;
			OVERLAPPED	m_readIO;
			OVERLAPPED	m_writeIO;
		};
	}
}


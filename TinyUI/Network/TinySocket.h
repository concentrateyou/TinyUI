#pragma once
#include "TinyNetwork.h"

namespace TinyUI
{
	namespace Network
	{
		/// <summary>
		/// Ì×½Ó×Ö¾ä±ú
		/// </summary>
		class TinyHandleSOCKET
		{
			DISALLOW_COPY_AND_ASSIGN(TinyHandleSOCKET)
		public:
			TinyHandleSOCKET();
			~TinyHandleSOCKET();
		public:
			operator SOCKET() const;
			SOCKET Handle() const;
			BOOL operator == (const TinyHandleSOCKET& obj) const;
			BOOL operator != (const TinyHandleSOCKET& obj) const;
			BOOL Attach(SOCKET socket);
			SOCKET Detach();
			TinyHandleSOCKET* Lookup(SOCKET socket);
		public:
			BOOL SetOption(INT level, INT opt, const CHAR* optval, INT size);
			BOOL GetOption(INT level, INT opt, CHAR* optval, INT& size);
		public:
			static BOOL GetAcceptEx(SOCKET socket, LPFN_ACCEPTEX* target);
			static BOOL GetConnectEx(SOCKET socket, LPFN_CONNECTEX* target);
			static BOOL GetAcceptExSockaddrs(SOCKET socket, LPFN_GETACCEPTEXSOCKADDRS* target);
			static BOOL GetDisconnectEx(SOCKET socket, LPFN_DISCONNECTEX* target);
		protected:
			SOCKET	m_socket;
			static  TinyPointerMap	m_socketMap;
		};
		class TinyIOServer;
		/// <summary>
		/// Socket(IOCP¿ò¼Ü)
		/// </summary>
		class TinySocket : public TinyHandleSOCKET
		{
			friend class TinyIOServer;
			friend class TinyIOTask;
			DISALLOW_COPY_AND_ASSIGN(TinySocket)
		public:
			class AcceptAsyncResult : public AsyncResult
			{
			public:
				virtual ~AcceptAsyncResult();
			public:
				TinySocket*	AcceptSocket;
			};
			class StreamAsyncResult : public AsyncResult
			{
			public:
				virtual ~StreamAsyncResult();
			public:
				WSABUF	Array;
				DWORD	BytesTransferred;
			};
			class DatagramAsyncResult : public StreamAsyncResult
			{
			public:
				virtual ~DatagramAsyncResult();
			public:
				SOCKADDR	Address;
				INT			Size;
			};
		public:
			TinySocket(TinyIOServer* ioserver = NULL);
			virtual ~TinySocket();
			TinyIOServer* GetIOServer() const;
			BOOL IsConnect() const;
			BOOL Open(INT addressFamily = AF_INET, INT socketType = SOCK_STREAM, INT protocolType = IPPROTO_TCP);
			BOOL SetKeepAlive(BOOL bAllow);
			BOOL IsKeepAlive();
			BOOL SetBlocking(BOOL bAllow);
			BOOL Duplicate(DWORD processID, WSAPROTOCOL_INFO& s);
			BOOL Available(INT& argp);
			INT	GetLastError();
		public:
			BOOL Bind(const IPEndPoint& endpoint);
			BOOL Listen(DWORD backlog = SOMAXCONN);
			TinySocket* Accept();
			BOOL Connect(const IPEndPoint& endpoint);
			INT  Receive(CHAR* data, DWORD dwSize, DWORD dwFlag);
			INT	 Send(CHAR* data, DWORD dwSize, DWORD dwFlag);
			INT	 ReceiveFrom(CHAR* data, DWORD dwSize, DWORD dwFlags, IPEndPoint& endpoint);
			INT	 SendTo(CHAR* data, DWORD dwSize, DWORD dwFlag, IPEndPoint& endpoint);
			BOOL Post(CompleteCallback&& callback, AsyncResult* result, LPVOID arg);
		public:
			BOOL BeginAccept(CompleteCallback&& callback, LPVOID arg);
			TinySocket* EndAccept(AsyncResult* result);
			BOOL BeginConnect(const IPEndPoint& endpoint, CompleteCallback&& callback, LPVOID arg);
			void EndConnect(AsyncResult* result);
			BOOL BeginSend(CHAR* data, DWORD dwSize, DWORD dwFlags, CompleteCallback&&callback, LPVOID arg);
			INT  EndSend(AsyncResult* result);
			BOOL BeginReceive(CHAR* data, DWORD dwSize, DWORD dwFlags, CompleteCallback&& callback, LPVOID arg);
			INT  EndReceive(AsyncResult* result);
			BOOL BeginSendTo(CHAR* data, DWORD dwSize, DWORD dwFlags, IPEndPoint& endpoint, CompleteCallback&& callback, LPVOID arg);
			INT  EndSendTo(AsyncResult* result);
			BOOL BeginReceiveFrom(CHAR* data, DWORD dwSize, DWORD dwFlags, CompleteCallback&& callback, LPVOID arg);
			INT  EndReceiveFrom(AsyncResult* result, IPEndPoint& endpoint);
			BOOL BeginDisconnect(CompleteCallback&& callback, LPVOID arg);
			void EndDisconnect(AsyncResult* result);
		public:
			virtual void Close();
			virtual BOOL Shutdown(INT how = SD_BOTH);
		private:
			static void CALLBACK AsyncCallback(PVOID pThis, BOOLEAN b);
		protected:
			TinyIOServer*		m_ioserver;
			LPFN_DISCONNECTEX	m_disconnectex;
			LPFN_CONNECTEX		m_connectex;
			LPFN_ACCEPTEX		m_acceptex;
			INT					m_addressFamily;
			INT					m_socketType;
			INT					m_protocolType;
			DWORD				m_dwMode;
			BOOL				m_connect;
			TinyLock			m_synclock;
		};
	}
}


#pragma once
#include "TinyNetwork.h"

namespace TinyUI
{
	namespace Network
	{
		class TinySocket;
		/// <summary>
		/// Ì×½Ó×Ö¾ä±ú
		/// </summary>
		class TinyHandleSOCKET
		{
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
			static  TinyPtrMap	m_socketMap;
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
		private:
			class AcceptAsyncResult : public AsyncResult
			{
			public:
				TinySocket*	AcceptSocket;
			};
			class StreamAsyncResult : public AsyncResult
			{
			public:
				WSABUF	Array;
				DWORD	BytesTransferred;
			};
			class DatagramAsyncResult : public StreamAsyncResult
			{
			public:
				SOCKADDR_IN	Address;
			};
		public:
			explicit TinySocket(TinyIOServer* ioserver);
			virtual ~TinySocket();
			BOOL	IsConnect() const;
			BOOL	Open(INT addressFamily = AF_INET, INT socketType = SOCK_STREAM, INT protocolType = IPPROTO_TCP);
			BOOL	KeepAlive(BOOL bAllow, INT ms);
			BOOL	IsKeepAlive();
			INT		Available();
			BOOL	Blocking(BOOL bAllow);
			BOOL	Duplicate(DWORD processID, WSAPROTOCOL_INFO& s);
		public:
			BOOL Bind(const IPAddress& address, USHORT sPORT);
			BOOL Listen(DWORD backlog = SOMAXCONN);
			BOOL BeginAccept(CompleteCallback& callback, LPVOID arg);
			TinySocket* EndAccept(AsyncResult* result);
			BOOL BeginConnect(IPAddress& address, USHORT sPORT, CompleteCallback& callback, LPVOID arg);
			void EndConnect(AsyncResult* result);
			BOOL BeginSend(CHAR* data, DWORD dwSize, DWORD dwFlags, CompleteCallback& callback, LPVOID arg);
			INT  EndSend(AsyncResult* result);
			BOOL BeginReceive(CHAR* data, DWORD dwSize, DWORD dwFlags, CompleteCallback& callback, LPVOID arg);
			INT  EndReceive(AsyncResult* result);
			BOOL BeginSendTo(CHAR* data, DWORD dwSize, DWORD dwFlags, const IPAddress& address, DWORD dwPORT, CompleteCallback& callback, LPVOID arg);
			INT  EndSendTo(AsyncResult* result);
			BOOL BeginReceiveFrom(CHAR* data, DWORD dwSize, DWORD dwFlags, CompleteCallback& callback, LPVOID arg);
			INT  EndReceiveFrom(AsyncResult* result, SOCKADDR_IN& si);
			BOOL BeginDisconnect(CompleteCallback& callback, LPVOID arg);
			void EndDisconnect(AsyncResult* result);
		public:
			virtual void Close();
			virtual BOOL Shutdown(INT how = SD_BOTH);
		protected:
			TinyIOServer*		m_server;
			INT					m_addressFamily;
			INT					m_socketType;
			INT					m_protocolType;
			BOOL				m_connect;
			LPFN_DISCONNECTEX	m_disconnectex;
			LPFN_CONNECTEX		m_connectex;
			LPFN_ACCEPTEX		m_acceptex;
			TinyLock			m_lock;
		};
	}
}


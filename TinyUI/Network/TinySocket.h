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
			static  TinyPointerMap	m_map;
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
			BOOL	IsEmpty() const;
			BOOL	IsConnect() const;
			BOOL	Open(INT addressFamily = AF_INET, INT socketType = SOCK_STREAM, INT protocolType = IPPROTO_TCP);
			BOOL	SetKeepAlive(BOOL bAllow);
			BOOL	GetKeepAlive(BOOL& bAllow);
			BOOL	SetDelay(BOOL bAllow);
			BOOL	GetDelay(BOOL& bAllow);
			BOOL	SetBlocking(BOOL bAllow);
			BOOL	GetTimeout(BOOL bRecv, DWORD& dwTime);
			BOOL	SetTimeout(BOOL bRecv, DWORD dwTime);
			BOOL	Duplicate(DWORD processID, WSAPROTOCOL_INFO& s);
			BOOL	Available(INT& argp);
			INT		GetLastError();
		public:
			TinySocket* Accept();
			BOOL	Poll(INT micros, INT mode);//read = 0,write = 1,error = 2
			BOOL	Bind(const IPEndPoint& endpoint);
			BOOL	Listen(DWORD backlog = SOMAXCONN);
			BOOL	Connect(const IPEndPoint& endpoint, DWORD dwS);
			INT		Receive(CHAR* data, DWORD dwSize, DWORD dwFlag = 0);
			INT		Send(CHAR* data, DWORD dwSize, DWORD dwFlag = 0);
			INT		ReceiveFrom(CHAR* data, DWORD dwSize, DWORD dwFlags, IPEndPoint& endpoint);
			INT		SendTo(CHAR* data, DWORD dwSize, DWORD dwFlag, IPEndPoint& endpoint);
			BOOL	Post(CompleteCallback&& callback, AsyncResult* result, LPVOID arg);
		public:
			BOOL	BeginAccept(CompleteCallback&& callback, LPVOID arg);
			TinySocket* EndAccept(AsyncResult* result);
			BOOL	BeginConnect(const IPEndPoint& endpoint, CompleteCallback&& callback, LPVOID arg);
			void	EndConnect(AsyncResult* result);
			BOOL	BeginDisconnect(CompleteCallback&& callback, LPVOID arg);
			void	EndDisconnect(AsyncResult* result);
			BOOL	BeginSend(CHAR* data, DWORD dwSize, DWORD dwFlags, CompleteCallback&&callback, LPVOID arg);
			DWORD	EndSend(AsyncResult* result);
			BOOL	BeginReceive(CHAR* data, DWORD dwSize, DWORD dwFlags, CompleteCallback&& callback, LPVOID arg);
			DWORD	EndReceive(AsyncResult* result);
			BOOL	BeginSendTo(CHAR* data, DWORD dwSize, DWORD dwFlags, IPEndPoint& endpoint, CompleteCallback&& callback, LPVOID arg);
			DWORD	EndSendTo(AsyncResult* result);
			BOOL	BeginReceiveFrom(CHAR* data, DWORD dwSize, DWORD dwFlags, CompleteCallback&& callback, LPVOID arg);
			DWORD	EndReceiveFrom(AsyncResult* result, IPEndPoint& endpoint);
		public:
			void	SetErrorCallback(Callback<void(INT)>&& callback);
			virtual void Close();
			virtual BOOL Shutdown(INT how = SD_BOTH);
		private:
			static void CALLBACK AsyncCallback(PVOID pThis, BOOLEAN b);
		protected:
			BOOL				m_connect;
			INT					m_addressFamily;
			INT					m_socketType;
			INT					m_protocolType;
			Callback<void(INT)>	m_errorCallback;
			TinyIOServer*		m_ioserver;
			LPFN_DISCONNECTEX	m_disconnectex;
			LPFN_CONNECTEX		m_connectex;
			LPFN_ACCEPTEX		m_acceptex;
			TinyLock			m_synclock;
		};
	}
}


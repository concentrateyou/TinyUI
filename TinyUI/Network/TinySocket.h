#pragma once
#include "TinyNetwork.h"

namespace TinyUI
{
	namespace Network
	{
		class IPAddress
		{
		public:
			enum : size_t { IPv4AddressSize = 4, IPv6AddressSize = 16 };
		public:
			IPAddress();
			explicit IPAddress(const vector<BYTE>& address);
			explicit IPAddress(const string& ip);
			IPAddress(IPAddress&& other);
			IPAddress(const IPAddress& other);
			template <INT N>
			IPAddress(const BYTE(&address)[N])
				: IPAddress(address, N)
			{}
			IPAddress(const BYTE* address, DWORD size);
			IPAddress(BYTE b0, BYTE b1, BYTE b2, BYTE b3);
			IPAddress(BYTE b0,
				BYTE b1,
				BYTE b2,
				BYTE b3,
				BYTE b4,
				BYTE b5,
				BYTE b6,
				BYTE b7,
				BYTE b8,
				BYTE b9,
				BYTE b10,
				BYTE b11,
				BYTE b12,
				BYTE b13,
				BYTE b14,
				BYTE b15);
			~IPAddress();
			BOOL operator==(const IPAddress& other) const;
			BOOL operator!=(const IPAddress& other) const;
			BOOL operator<(const IPAddress& other) const;
			BOOL IsIPv4() const;
			BOOL IsIPv6() const;
			BOOL IsValid() const;
			BOOL IsZero() const;
			BOOL IsEmpty() const;
			std::string ToString() const;
			DWORD Size() const;
			const std::vector<BYTE>& Address() const;
			static IPAddress IPv4Any();
			static IPAddress IPv6Any();
		private:
			std::vector<BYTE> m_address;
		};
		using IPAddressList = std::vector<IPAddress>;
		enum AddressFamily
		{
			ADDRESS_FAMILY_UNSPECIFIED,
			ADDRESS_FAMILY_IPV4,
			ADDRESS_FAMILY_IPV6,
			ADDRESS_FAMILY_LAST = ADDRESS_FAMILY_IPV6
		};
		class TinySocket;
		using CompleteCallback = Callback<void(DWORD, DWORD, LPVOID)>;
		class NO_VTABLE PER_IO_CONTEXT : public OVERLAPPED
		{
		public:
			PER_IO_CONTEXT();
			void Reset();
		public:
			DWORD				OP;
			DWORD				Bytes;
			LPVOID				AsyncState;
			WSABUF				Buffer;
			CompleteCallback	Complete;
		};
		/// <summary>
		/// 套接字句柄
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
			BOOL SetOption(INT opt, const CHAR* optval, INT size);
			BOOL GetOption(INT opt, CHAR* optval, INT& size);
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
		/// 套接字基类
		/// </summary>
		class TinySocket : public TinyHandleSOCKET
		{
			friend class TinyIOServer;
			friend class TinyIOTask;
			DISALLOW_COPY_AND_ASSIGN(TinySocket)
		public:
			explicit TinySocket(TinyIOServer* ioserver);
			virtual ~TinySocket();
			BOOL	IsConnect() const;
			BOOL	Open(INT addressFamily = AF_INET, INT socketType = SOCK_STREAM, INT protocolType = IPPROTO_TCP);
		public:
			BOOL Bind(const IPAddress& address, DWORD dwPORT);
			BOOL Listen(DWORD backlog = SOMAXCONN);
			BOOL BeginAccept(CompleteCallback& callback, LPVOID arg);
			BOOL BeginConnect(IPAddress& address, DWORD dwPORT, CompleteCallback& callback, LPVOID arg);
			BOOL BeginReceive(CHAR* data, DWORD dwSize, DWORD dwFlags, CompleteCallback& callback, LPVOID arg);
			BOOL BeginDisconnect(CompleteCallback& callback, LPVOID arg);
		public:
			virtual void Close();
			virtual BOOL Shutdown(INT how);
		public:
		protected:
			TinyIOServer*		m_server;
			INT					m_addressFamily;
			INT					m_socketType;
			INT					m_protocolType;
			BOOL				m_connect;
			PER_IO_CONTEXT		m_context;
			LPFN_DISCONNECTEX	m_disconnectex;
			LPFN_CONNECTEX		m_connectex;
			LPFN_ACCEPTEX		m_acceptex;
		};
	}
}


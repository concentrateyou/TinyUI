#pragma once
#include "../Common/TinyCollection.h"
#include "../IO/TinyIO.h"
#include "../Common/TinyCallback.h"
#include "../IO/TinyIOCP.h"
#include "../IO/TinyThread.h"
#include <WinSock2.h>
#include <mswsock.h>
#include <Ws2ipdef.h>
#include <WS2tcpip.h>
#include <Websocket.h>
#pragma comment(lib,"Mswsock.lib")
#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"Websocket.lib")

namespace TinyFramework
{
	namespace Network
	{
		BOOL  GetExtensionPtr(SOCKET socket, GUID guid, void** target);
		DWORD GetErrorCode(SOCKET socket, LPOVERLAPPED ps);
		BOOL GetIPAddressFromSOCKADDR(const SOCKADDR* address, INT addresssize, const BYTE** addressdata, size_t* datasize, USHORT* port);
		enum AddressFamily
		{
			ADDRESS_FAMILY_UNSPECIFIED,   // AF_UNSPEC
			ADDRESS_FAMILY_IPV4,          // AF_INET
			ADDRESS_FAMILY_IPV6,          // AF_INET6
			ADDRESS_FAMILY_LAST = ADDRESS_FAMILY_IPV6
		};
#define OP_UNKNOW		0x00
#define OP_ACCEPT		0x01
#define OP_RECV			0x02
#define OP_RECVFROM		0x03
#define OP_SEND			0x04
#define OP_SENDTO		0x05
#define OP_CONNECT		0x06
#define OP_DISCONNECT	0x07
#define OP_ERROR		0x08
#define OP_QUIT			0x09
		/// <summary>
		/// IP地址
		/// </summary>
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
			IPAddress& operator =(const IPAddress& other);
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
			INT	AddressFamily() const;
			std::string ToString() const;
			size_t size() const;
			const std::vector<BYTE>& address() const;
			static IPAddress IPv4Any();
			static IPAddress IPv6Any();
		private:
			std::vector<BYTE> m_address;
		};
		using IPAddressList = std::vector<IPAddress>;
		AddressFamily GetAddressFamily(const IPAddress& address);
		INT ConvertAddressFamily(AddressFamily addressFamily);
		/// <summary>
		/// 网络终结点
		/// </summary>
		class  IPEndPoint
		{
		public:
			IPEndPoint();
			~IPEndPoint();
			IPEndPoint(const IPAddress& address, USHORT port);
			IPEndPoint(const IPEndPoint& endpoint);
			const IPAddress& address() const;
			INT	AddressFamily() const;
			USHORT port() const;
			BOOL ToSOCKADDR(SOCKADDR* address, size_t* size) const;
			BOOL FromIPAddress(const IPAddress& address, SHORT port);
			BOOL FromSOCKADDR(const  SOCKADDR* address, size_t size);
			std::string ToString() const;
			BOOL operator<(const IPEndPoint& other) const;
			BOOL operator==(const IPEndPoint& other) const;
		private:
			IPAddress	m_address;
			USHORT		m_port;
		};
		/// <summary>
		/// 地址集合
		/// </summary>
		class AddressList
		{
		public:
			AddressList();
			~AddressList();
			explicit AddressList(const IPEndPoint& endpoint);
			void SetCanonical(const std::string& canonical);
			static AddressList CreateFromIPAddress(const IPAddress& address, USHORT port);
			static AddressList CreateFromIPAddressList(const IPAddressList& addresses, const std::string& canonical);
			static AddressList CreateFromAddrinfo(const ADDRINFO* head);
			using iterator = std::vector<IPEndPoint>::iterator;
			using const_iterator = std::vector<IPEndPoint>::const_iterator;
			size_t size() const;
			bool empty() const;
			void clear();
			void reserve(size_t count);
			size_t capacity() const;
			IPEndPoint& operator[](size_t index);
			const IPEndPoint& operator[](size_t index) const;
			IPEndPoint& front();
			const IPEndPoint& front() const;
			IPEndPoint& back();
			const IPEndPoint& back() const;
			void push_back(const IPEndPoint& val);
			template <typename InputIt>
			void insert(iterator pos, InputIt first, InputIt last)
			{
				m_endpoints.insert(pos, first, last);
			}
			iterator begin();
			const_iterator begin() const;
			iterator end();
			const_iterator end() const;
		private:
			std::vector<IPEndPoint> m_endpoints;
			std::string				m_canonical;
		};
		/// <summary>
		/// 异步模型
		/// </summary>
		class AsyncResult : public TinyReference<AsyncResult>
		{
		public:
			AsyncResult();
			virtual ~AsyncResult();
		public:
			LPVOID	AsyncState;
			HANDLE	AsyncHandle;
		};
		using CompleteCallback = Callback<void(INT, AsyncResult*)>;
		using CompletionCallback = Callback<void(INT, INT)>;
		using ErrorCallback = Callback<void(INT)>;
		/// <summary>
		/// OVERLAPPED拓展结构
		/// </summary>
		class  PER_IO_CONTEXT : public WSAOVERLAPPED
		{
		public:
			PER_IO_CONTEXT();
			~PER_IO_CONTEXT();
			void Reset();
		public:
			DWORD								OP;
			LONG_PTR							Context;
			TinyScopedReferencePtr<AsyncResult>	Result;
			CompleteCallback					Complete;
		};
		/// <summary>
		/// 套接字句柄
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
	}
}


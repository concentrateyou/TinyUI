#pragma once
#include "../Common/TinyCollection.h"
#include "../IO/TinyIO.h"
#include "../Common/TinyCallback.h"
#include "../IO/TinyIOCP.h"
#include "../IO/TinyTaskBase.h"
#include <WinSock2.h>
#include <mswsock.h>
#include <Ws2ipdef.h>
#include <WS2tcpip.h>
#include <Websocket.h>
#pragma comment(lib,"Mswsock.lib")
#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"Websocket.lib")

namespace TinyUI
{
	namespace Network
	{
		BOOL  GetExtensionPtr(SOCKET socket, GUID guid, void** target);
		DWORD GetErrorCode(SOCKET socket, LPOVERLAPPED ps);
		BOOL GetIPAddressFromSOCKADDR(const SOCKADDR* address, INT addresssize, const BYTE** addressdata, size_t* datasize, USHORT* port);;
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
		using CompleteCallback = Callback<void(DWORD, AsyncResult*)>;
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
	}
}


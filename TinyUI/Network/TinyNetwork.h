#pragma once
#include "../Common/TinyCollection.h"
#include "../IO/TinyIO.h"
#include "../Common/TinyCallback.h"
#include "../IO/TinyIOCP.h"
#include "../IO/TinyTaskBase.h"
#include <WinSock2.h>
#include <mswsock.h>
#pragma comment(lib,"Mswsock.lib")
#pragma comment(lib,"Ws2_32.lib")

namespace TinyUI
{
	namespace Network
	{
		BOOL GetExtensionPtr(SOCKET socket, GUID guid, void** target);
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
			HANDLE	AsyncHandle;//预留
		};
		using CompleteCallback = Callback<void(DWORD, AsyncResult*)>;
		/// <summary>
		/// OVERLAPPED拓展结构
		/// </summary>
		class NO_VTABLE PER_IO_CONTEXT : public OVERLAPPED
		{
		public:
			PER_IO_CONTEXT();
			~PER_IO_CONTEXT();
			void Reset();
		public:
			DWORD								OP;
			LONG_PTR							Reserve;
			TinyScopedReferencePtr<AsyncResult>	Result;
			CompleteCallback					Complete;
		};
	}
}


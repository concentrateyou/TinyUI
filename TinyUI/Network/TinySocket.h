#pragma once
#include "TinyNetwork.h"

namespace TinyUI
{
	namespace Network
	{
		class IPAddress
		{
			enum : size_t { IPv4AddressSize = 4, IPv6AddressSize = 16 };
		public:
			IPAddress();
			explicit IPAddress(const vector<BYTE>& address);
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
		private:
			std::vector<uint8_t> m_address;
		};
		using IPAddressList = std::vector<IPAddress>;
		enum AddressFamily
		{
			ADDRESS_FAMILY_UNSPECIFIED,
			ADDRESS_FAMILY_IPV4,
			ADDRESS_FAMILY_IPV6,
			ADDRESS_FAMILY_LAST = ADDRESS_FAMILY_IPV6
		};

		class NO_VTABLE PER_IO_CONTEXT : public OVERLAPPED
		{
			using IOCompletionCallback = Callback<void(DWORD, DWORD, PER_IO_CONTEXT*)>;
		public:
			PER_IO_CONTEXT();
			~PER_IO_CONTEXT();
		public:
			DWORD					OP;
			SOCKET					AcceptSocket;
			SOCKET					ListenSocket;
			WSABUF					Buffer;
			DWORD					NumberOfBytes;
			DWORD					NumberOfBytesTransferred;
			IOCompletionCallback	IOCompletion;
		public:
			void Reset();
			void Destory();
		};

		using CompleteCallback = Callback<void(DWORD, DWORD)>;

		/// <summary>
		/// Ì×½Ó×Ö
		/// </summary>
		class TinySocket
		{
		public:
			static BOOL GetAcceptEx(SOCKET socket, LPFN_ACCEPTEX* target);
			static BOOL GetConnectEx(SOCKET socket, LPFN_CONNECTEX* target);
			static BOOL GetAcceptExSockaddrs(SOCKET socket, LPFN_GETACCEPTEXSOCKADDRS* target);
			static BOOL GetDisconnectEx(SOCKET socket, LPFN_DISCONNECTEX* target);
		public:
			TinySocket(SOCKET socket);
			virtual ~TinySocket();
			operator SOCKET() const;
			virtual void Close();
			virtual BOOL Shutdown(INT how);
		protected:
			SOCKET			m_socket;
			PER_IO_CONTEXT	m_io;
		};
	}
}


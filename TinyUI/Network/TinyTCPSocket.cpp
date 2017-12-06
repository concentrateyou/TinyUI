#include "../stdafx.h"
#include "TinyTCPSocket.h"

namespace TinyUI
{
	namespace Network
	{
		TinyTCPSocket::TinyTCPSocket()
		{

		}
		TinyTCPSocket::~TinyTCPSocket()
		{

		}
		BOOL TinyTCPSocket::Open(AddressFamily addressFamily)
		{
			BOOL bRes = this->Attach(::WSASocket(ConvertAddressFamily(addressFamily), SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED));
			return bRes;
		}
	}
}
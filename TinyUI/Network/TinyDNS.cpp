#include "../stdafx.h"
#include "TinyDNS.h"

namespace TinyUI
{
	namespace Network
	{
		TinyDNS::TinyDNS()
		{

		}
		BOOL TinyDNS::Resolver(const string& host, const string& service, AddressList& list, INT addressFamily)
		{
			list.clear();
			ADDRINFO* ai = NULL;
			ADDRINFO hints = { 0 };
			hints.ai_family = addressFamily;
			hints.ai_flags |= AI_CANONNAME;
			hints.ai_socktype = SOCK_STREAM;
			INT iRes = getaddrinfo(host.c_str(), service.c_str(), &hints, &ai);
			if (iRes == ERROR_SUCCESS)
			{
				list = AddressList::CreateFromAddrinfo(ai);
			}
			freeaddrinfo(ai);
			return iRes == ERROR_SUCCESS;
		}
	}
}
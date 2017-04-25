#pragma once
#include "TinySocket.h"

namespace TinyUI
{
	namespace Network
	{
		/// <summary>
		/// DNS����
		/// </summary>
		class TinyDNS
		{
		public:
			TinyDNS();
		public:
			BOOL Resolver(const string& host, const string& service, AddressList& list, INT addressFamily = AF_UNSPEC);	
		};
	}
}


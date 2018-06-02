#pragma once
#include "TinySocket.h"
#include <WinDNS.h>

namespace TinyFramework
{
	namespace Network
	{
		/// <summary>
		/// DNS解析
		/// </summary>
		class TinyDNS
		{
		public:
			TinyDNS();
		public:
			/// <summary>
			/// 同步
			/// </summary>
			BOOL Resolver(const string& host, const string& service, AddressList& list, INT addressFamily = AF_UNSPEC);
			/// <summary>
			/// 异步
			/// </summary>
			BOOL BeginResolver(PCWSTR queryName, PSTR serverIP, PDNS_ADDR_ARRAY dnsServerList);
		private:
			static VOID WINAPI QueryCompleteCallback(PVOID Context, PDNS_QUERY_RESULT QueryResults);
		};
	}
}


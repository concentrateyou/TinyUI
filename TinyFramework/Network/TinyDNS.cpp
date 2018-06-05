#include "../stdafx.h"
#include "TinyDNS.h"

namespace TinyFramework
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
		VOID WINAPI TinyDNS::QueryCompleteCallback(PVOID Context, PDNS_QUERY_RESULT QueryResults)
		{
			TinyDNS* pThis = reinterpret_cast<TinyDNS*>(Context);
			if (pThis != NULL)
			{

			}
		}
		BOOL TinyDNS::BeginResolver(PCWSTR queryName, PSTR serverIp, PDNS_ADDR_ARRAY dnsServerList)
		{
			DNS_QUERY_REQUEST request;
			ZeroMemory(&request, sizeof(request));
			request.Version = DNS_QUERY_REQUEST_VERSION1;
			request.QueryType = DNS_TYPE_A;
			request.QueryOptions = 0;
			request.QueryName = queryName;
			request.pQueryContext = this;
			request.pQueryCompletionCallback = TinyDNS::QueryCompleteCallback;

			DWORD  dwError = ERROR_SUCCESS;
			SOCKADDR_STORAGE address;
			INT size = sizeof(address);
			WSADATA wsaData;
			ZeroMemory(&wsaData, sizeof(wsaData));
			ZeroMemory(dnsServerList, sizeof(*dnsServerList));
			dwError = WSAStringToAddress(serverIp,
				AF_INET,
				NULL,
				(LPSOCKADDR)&address,
				&size);
			if (dwError != ERROR_SUCCESS)
			{
				size = sizeof(address);
				dwError = WSAStringToAddress(serverIp,
					AF_INET6,
					NULL,
					(LPSOCKADDR)&address,
					&size);
			}
			if (dwError != ERROR_SUCCESS)
			{
				return FALSE;
			}
			dnsServerList->MaxCount = 1;
			dnsServerList->AddrCount = 1;
			CopyMemory(dnsServerList->AddrArray[0].MaxSa, &address, DNS_ADDR_MAX_SOCKADDR_LENGTH);
		}
	}
}
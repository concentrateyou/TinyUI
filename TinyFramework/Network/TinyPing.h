#pragma once
#include "TinySocket.h"

namespace TinyFramework
{
	namespace Network
	{
		/// <summary>
		/// Ping
		/// </summary>
		class TinyPing
		{
		public:
			TinyPing();
			virtual ~TinyPing();
			BOOL IsEmpty() const;
			DWORD Ping(IPAddress& address, WORD wRequestSize, DWORD dwTimeout, BYTE ttl, BOOL allow);
			BOOL EstimateMTU(IPAddress& address, UINT16* pMTU);
		private:
			HANDLE				m_handle;
			TinyBuffer<CHAR>	m_request;
			TinyBuffer<CHAR>	m_replay;
		};
	}
}


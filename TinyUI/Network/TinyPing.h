#pragma once
#include "TinySocket.h"

namespace TinyUI
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
			BOOL Ping(IPAddress& address, WORD wRequestSize, DWORD dwTimeout, BYTE ttl, BOOL allow);
		private:
			HANDLE				m_handle;
			TinyBuffer<CHAR>	m_request;
			TinyBuffer<CHAR>	m_replay;
		};
	}
}


#pragma once
#include "TinySocket.h"

namespace TinyUI
{
	namespace Network
	{
		/// <summary>
		/// TCP�׽���
		/// </summary>
		class TinyTCPSocket : public TinyHandleSOCKET
		{
			DISALLOW_COPY_AND_ASSIGN(TinyTCPSocket)
		public:
			TinyTCPSocket();
			virtual ~TinyTCPSocket();
		public:
			BOOL Open(AddressFamily addressFamily);
		};
	}
}


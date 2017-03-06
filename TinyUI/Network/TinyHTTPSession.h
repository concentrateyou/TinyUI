#pragma once
#include "TinySocket.h"

namespace TinyUI
{
	namespace Network
	{
		class TinyHTTPSession : public TinyReference <TinyHTTPSession>
		{
			DISALLOW_COPY_AND_ASSIGN(TinyHTTPSession)
		public:
			TinyHTTPSession();
			virtual ~TinyHTTPSession();
		protected:
			TinySocket	m_socket;
		};
	}
}


#include "../stdafx.h"
#include "TinyAcceptor.h"
#include <process.h>

namespace TinyUI
{
	namespace Network
	{
		TinyAcceptor::TinyAcceptor(SOCKET listen)
			:m_listen(listen)
		{

		}
		BOOL TinyAcceptor::BeginAccept(SOCKET socket)
		{
			ASSERT(socket);
			LPFN_ACCEPTEX acceptex = NULL;
			if (TinySocket::GetAcceptEx(socket, &acceptex))
			{
				ACCEPT_IO_CONTEXT* context = new ACCEPT_IO_CONTEXT();
				context->OP = OP_ACCEPT;
				context->socket = socket;
			}
		}
		ACCEPT_IO_CONTEXT* TinyAcceptor::EndAccept()
		{
			return NULL;
		}
		void TinyAcceptor::OnCompletionStatus(IO_CONTEXT* pIO, DWORD dwError)
		{

		}
	}
}
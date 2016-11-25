#include "../stdafx.h"
#include "TinyConnector.h"
#include <process.h>

namespace TinyUI
{
	namespace Network
	{
		TinyConnector::TinyConnector(SOCKET socket)
			:TinySocket(socket)
		{

		}
		TinyConnector::~TinyConnector()
		{

		}
		BOOL TinyConnector::BeginConnect(SOCKADDR_IN address)
		{
			return FALSE;
		}
	}
}
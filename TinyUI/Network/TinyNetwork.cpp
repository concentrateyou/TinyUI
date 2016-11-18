#include "../stdafx.h"
#include "TinyNetwork.h"
#include <process.h>

namespace TinyUI
{
	namespace Network
	{
		BOOL GetExtensionPtr(SOCKET socket, GUID guid, void **target)
		{
			DWORD dwBytes = 0;
			if (WSAIoctl(socket,
				SIO_GET_EXTENSION_FUNCTION_POINTER,
				&guid,
				sizeof(guid),
				(void*)target,
				sizeof(*target),
				&dwBytes,
				NULL,
				NULL) == SOCKET_ERROR)
			{
				*target = NULL;
				return FALSE;
			}
			return TRUE;
		}
	}
}
#pragma once
#include "../Common//TinyCollection.h"
#include "../IO/TinyIO.h"
#include "../Common/TinyCallback.h"
#include "../IO/TinyIOCP.h"
#include "../IO/TinyTaskBase.h"
#include <WinSock2.h>
#include <mswsock.h>
#pragma comment(lib,"Mswsock.lib")
#pragma comment(lib,"Ws2_32.lib")

namespace TinyUI
{
	namespace Network
	{
		BOOL GetExtensionPtr(SOCKET socket, GUID guid, void** target);
	}
}


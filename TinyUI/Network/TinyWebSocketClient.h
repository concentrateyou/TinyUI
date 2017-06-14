#pragma once
#include "TinySocket.h"


namespace TinyUI
{
	namespace Network
	{
#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)
		/// <summary>
		/// Win32 WebSocket½Ó¿Ú
		/// </summary>
		class TinyWebSocketClient
		{
		public:
			TinyWebSocketClient();
			virtual ~TinyWebSocketClient();
			BOOL Open(const vector<WEB_SOCKET_PROPERTY>& prop);
			BOOL Connect();
			void Close();
		protected:
			WEB_SOCKET_HANDLE m_client;
		};
#endif
	}
}


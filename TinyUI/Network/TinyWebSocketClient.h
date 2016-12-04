#pragma once
#include "TinySocket.h"


namespace TinyUI
{
	namespace Network
	{
		/// <summary>
		/// Win32 WebSocket�ӿ�
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
	}
}


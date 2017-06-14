#include "../stdafx.h"
#include "TinyWebSocketClient.h"

namespace TinyUI
{
	namespace Network
	{
#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)
		TinyWebSocketClient::TinyWebSocketClient()
			:m_client(NULL)
		{

		}
		TinyWebSocketClient::~TinyWebSocketClient()
		{

		}
		BOOL TinyWebSocketClient::Open(const vector<WEB_SOCKET_PROPERTY>& prop)
		{
			HRESULT hRes = WebSocketCreateClientHandle(const_cast<PWEB_SOCKET_PROPERTY>(&prop[0]), prop.size(), &m_client);
			if (hRes != S_OK)
				return FALSE;
			return TRUE;
		}
		void TinyWebSocketClient::Close()
		{
			if (m_client != NULL)
			{
				WebSocketDeleteHandle(m_client);
				m_client = NULL;
			}
		}
		BOOL TinyWebSocketClient::Connect()
		{
			ASSERT(m_client);
			WebSocketBeginClientHandshake(m_client, NULL, 0, NULL, 0, NULL, 0, NULL, NULL);
			return TRUE;
		}
#endif
	}
}
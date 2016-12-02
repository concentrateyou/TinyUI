#include "../stdafx.h"
#include "TinyHTTPClient.h"

namespace TinyUI
{
	namespace Network
	{
		TinyHTTPClient::TinyHTTPClient(const string& strURL, USHORT sPORT)
			:m_strURL(std::move(strURL)),
			m_sPORT(sPORT)
		{

		}
		TinyHTTPClient::~TinyHTTPClient()
		{

		}
	}
}
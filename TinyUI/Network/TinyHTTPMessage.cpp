#include "../stdafx.h"
#include "TinyHTTPMessage.h"

namespace TinyUI
{
	namespace Network
	{
		TinyHTTPMessage::TinyHTTPMessage()
		{

		}

		TinyHTTPMessage::~TinyHTTPMessage()
		{

		}

		void TinyHTTPMessage::Add(const string& key, const string& value)
		{
			m_map[key] = value;
		}

		void TinyHTTPMessage::Remove(const string& key)
		{
			m_map.erase(key);
		}
	}
}
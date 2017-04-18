#include "../stdafx.h"
#include "TinyURL.h"

namespace TinyUI
{
	namespace Network
	{
		TinyURL::TinyURL()
		{

		}

		BOOL TinyURL::ParseURL(const CHAR* val, DWORD size)
		{
			BOOL secure = FALSE;
			if (strncasecmp(val, "http://", 7) == 0)
			{
				val += 7;
				size -= 7;
				secure = FALSE;
			}
			else if (strncasecmp(val, "https://", 8) == 0)
			{
				val += 8;
				size -= 8;
				secure = TRUE;
			}
			else
			{

			}
			return FALSE;
		}
	}
}
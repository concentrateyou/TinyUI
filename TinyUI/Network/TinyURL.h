#pragma once
#include "TinySocket.h"

namespace TinyUI
{
	namespace Network
	{
		/// <summary>
		/// URL����
		/// </summary>
		class TinyURL
		{
		public:
			TinyURL();
			BOOL ParseURL(const CHAR* val, DWORD size);
		protected:
			 
		};
	}
}


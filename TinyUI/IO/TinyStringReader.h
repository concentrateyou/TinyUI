#pragma once
#include "../Common/TinyCollection.h"
#include "../Common/TinyCore.h"
#include "../Common/TinyCallback.h"
#include "../Common/TinyEvent.h"
#include <string>
#include <algorithm>
using namespace std;

namespace TinyUI
{
	namespace IO
	{
		/// <summary>
		/// ×Ö·û´®¶ÁÈ¡
		/// </summary>
		class TinyStringReader
		{
			DISALLOW_COPY_AND_ASSIGN(TinyStringReader)
		public:
			TinyStringReader(const string& Mystr);
			virtual ~TinyStringReader();
			BOOL	ReadLine(CHAR*& line1, CHAR*& line2);
			string	ReadLine();
		private:
			string				m_Mystr;
			string::size_type	m_Mypos;
		};
	};
}


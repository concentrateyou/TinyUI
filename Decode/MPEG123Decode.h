#pragma once
#include "Utility.h"
#include "mpg123.h"

namespace Decode
{
	class MPEG123Decode
	{
	public:
		MPEG123Decode();
		virtual ~MPEG123Decode();
	public:
		BOOL Open(LPCSTR pzFile);
		BOOL Decode(BYTE* data, LONG size);
		BOOL Close();
	private:
		mpg123_handle* m_handle;
	};
}




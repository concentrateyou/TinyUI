#pragma once
#include "Common.h"
#include "AACDecode.h"
#include "MPG123Decode.h"
#include "H264Decode.h"
#include <vector>

namespace Decode
{
	/// <summary>
	/// FLV°ü×°Æ÷
	/// </summary>
	class FLVWriter
	{
		DISALLOW_COPY_AND_ASSIGN(FLVWriter)
	public:
		FLVWriter();
		virtual ~FLVWriter();
		BOOL Create(LPCSTR pzFile);
		BOOL Close();
	private:
		TinyComPtr<IStream>		m_stream;
	};
}


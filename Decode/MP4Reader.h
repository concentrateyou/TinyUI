#pragma once
#include "Common.h"
#include "RTMPStream.h"
#include "HTTPStream.h"
#include "AACDecoder.h"
#include "x264Decoder.h"
#include <vector>

namespace Decode
{
	/// <summary>
	/// MP4ÎÄ¼þ½âÎö
	/// </summary>
	class MP4Reader
	{
		DISALLOW_COPY_AND_ASSIGN(MP4Reader)
	public:
		MP4Reader();
		virtual ~MP4Reader();
		BOOL		OpenFile(LPCSTR pzFile);
	private:
		TinySize				m_size;
		TinyComPtr<IStream>		m_stream;
	};
}



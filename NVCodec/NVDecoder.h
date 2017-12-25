#pragma once
#include "NVCodecCommon.h"

namespace NVCodec
{
	class NVDecoder
	{
		DISALLOW_COPY_AND_ASSIGN(NVDecoder)
	public:
		NVDecoder();
		virtual ~NVDecoder();
		BOOL Open(const BYTE* bits, LONG size);
		BOOL Decode(Media::SampleTag& tag);
		void Close();
	private:
		CUvideodecoder	m_decoder;
	};
}



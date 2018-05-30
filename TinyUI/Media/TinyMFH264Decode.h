#pragma once
#include "TinyMFDecode.h"

namespace TinyUI
{
	namespace Media
	{
		/// <summary>
		/// H264����
		/// CLSID_CMSH264DecoderMFT
		/// </summary>
		class TinyMFH264Decode : public TinyMFDecode
		{
			DISALLOW_COPY_AND_ASSIGN(TinyMFH264Decode)
		public:
			TinyMFH264Decode();
			virtual ~TinyMFH264Decode();
		public:
			BOOL SetFormat();
		};
	};
}

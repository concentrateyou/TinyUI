#pragma once
#include "TinyMFDecode.h"

namespace TinyUI
{
	namespace Media
	{
		/// <summary>
		/// Intel QSV H264½âÂë
		/// CLSID_MF_INTEL_H264DecFilter
		/// </summary>
		class TinyMFIntelQSVDecode : public TinyMFDecode
		{
			DISALLOW_COPY_AND_ASSIGN(TinyMFIntelQSVDecode)
		public:
			TinyMFIntelQSVDecode();
			virtual ~TinyMFIntelQSVDecode();
		public:
			BOOL SetFormat(const TinySize& size, DWORD dwFrameRate);
		};
	};
}

#pragma once
#include "TinyMFDecode.h"

namespace TinyUI
{
	namespace Media
	{
		/// <summary>
		/// Intel QSV H264½âÂë
		/// </summary>
		class TinyMFIntelQSVDecode : public TinyMFDecode
		{
			DISALLOW_COPY_AND_ASSIGN(TinyMFIntelQSVDecode)
		public:
			TinyMFIntelQSVDecode();
			virtual ~TinyMFIntelQSVDecode();
		public:
			BOOL Open(const TinySize& size, DWORD dwFrameRate, Callback<void(BYTE*, LONG, LPVOID)>&& callback);
		};
	};
}

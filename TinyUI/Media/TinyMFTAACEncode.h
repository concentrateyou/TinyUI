#pragma once
#include "TinyMFTDecode.h"

namespace TinyUI
{
	namespace Media
	{
		/// <summary>
		/// AAC±àÂë
		/// </summary>
		class TinyMFTAACEncode : public TinyMFTDecode
		{
			DISALLOW_COPY_AND_ASSIGN(TinyMFTAACEncode)
		public:
			TinyMFTAACEncode();
			virtual ~TinyMFTAACEncode();
		public:
			BOOL Open(const WAVEFORMATEX* pFMT, Callback<void(BYTE*, LONG, LPVOID)>&& callback);
		protected:

		};
	};
}

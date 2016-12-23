#pragma once
#include "TinyMFDecode.h"

namespace TinyUI
{
	namespace Media
	{
		/// <summary>
		/// AAC±àÂë
		/// </summary>
		class TinyMFAACEncode : public TinyMFDecode
		{
			DISALLOW_COPY_AND_ASSIGN(TinyMFAACEncode)
		public:
			TinyMFAACEncode();
			virtual ~TinyMFAACEncode();
		public:
			BOOL Open(const WAVEFORMATEX* pFMT, Callback<void(BYTE*, LONG, LPVOID)>&& callback);
		protected:

		};
	};
}

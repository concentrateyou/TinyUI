#pragma once
#include "TinyMFEncode.h"

namespace TinyUI
{
	namespace Media
	{
		/// <summary>
		/// AAC±àÂë
		/// </summary>
		class TinyMFAACEncode : public TinyMFEncode
		{
			DISALLOW_COPY_AND_ASSIGN(TinyMFAACEncode)
		public:
			TinyMFAACEncode();
			virtual ~TinyMFAACEncode();
		public:
			BOOL Open(const WAVEFORMATEX* pFMT, Callback<void(BYTE*, LONG, LPVOID)>&& callback);
		};
	};
}

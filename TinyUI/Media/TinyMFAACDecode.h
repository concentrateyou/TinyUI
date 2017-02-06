#pragma once
#include "TinyMFDecode.h"

namespace TinyUI
{
	namespace Media
	{
		/// <summary>
		/// AAC½âÂë
		/// </summary>
		class TinyMFAACDecode : public TinyMFDecode
		{
			DISALLOW_COPY_AND_ASSIGN(TinyMFAACDecode)
		public:
			TinyMFAACDecode();
			virtual ~TinyMFAACDecode();
		public:
			BOOL Open(const WAVEFORMATEX* pFMT, Callback<void(BYTE*, LONG, LPVOID)>&& callback);
		};
	};
}

#pragma once
#include "TinyMFTDecode.h"

namespace TinyUI
{
	namespace Media
	{
		/// <summary>
		/// AAC½âÂë
		/// </summary>
		class TinyMFTAACDecode : public TinyMFTDecode
		{
			DISALLOW_COPY_AND_ASSIGN(TinyMFTAACDecode)
		public:
			TinyMFTAACDecode();
			virtual ~TinyMFTAACDecode();
		public:
			BOOL Open(const WAVEFORMATEX* pFMT, Callback<void(BYTE*, LONG, LPVOID)>&& callback);
		protected:

		};
	};
}

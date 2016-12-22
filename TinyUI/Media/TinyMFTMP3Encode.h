#pragma once
#include "TinyMFTDecode.h"

namespace TinyUI
{
	namespace Media
	{
		/// <summary>
		/// MP3±àÂë
		/// </summary>
		class TinyMFTMP3Encode : public TinyMFTDecode
		{
			DISALLOW_COPY_AND_ASSIGN(TinyMFTMP3Encode)
		public:
			TinyMFTMP3Encode();
			virtual ~TinyMFTMP3Encode();
		public:
			BOOL Open(const WAVEFORMATEX* pFMT, Callback<void(BYTE*, LONG, LPVOID)>&& callback);
		protected:

		};
	};
}

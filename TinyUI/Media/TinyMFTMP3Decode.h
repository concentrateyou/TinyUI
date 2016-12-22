#pragma once
#include "TinyMFTDecode.h"

namespace TinyUI
{
	namespace Media
	{
		/// <summary>
		/// MP3½âÂë
		/// </summary>
		class TinyMFTMP3Decode : public TinyMFTDecode
		{
			DISALLOW_COPY_AND_ASSIGN(TinyMFTMP3Decode)
		public:
			TinyMFTMP3Decode();
			virtual ~TinyMFTMP3Decode();
		public:
			BOOL Open(const WAVEFORMATEX* pFMT, DWORD dwSampleRate, Callback<void(BYTE*, LONG, LPVOID)>&& callback);
		protected:

		};
	};
}

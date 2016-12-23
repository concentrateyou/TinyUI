#pragma once
#include "TinyMFDecode.h"

namespace TinyUI
{
	namespace Media
	{
		/// <summary>
		/// MP3½âÂë
		/// </summary>
		class TinyMFMP3Decode : public TinyMFDecode
		{
			DISALLOW_COPY_AND_ASSIGN(TinyMFMP3Decode)
		public:
			TinyMFMP3Decode();
			virtual ~TinyMFMP3Decode();
		public:
			BOOL Open(const WAVEFORMATEX* pFMT, DWORD dwSampleRate, Callback<void(BYTE*, LONG, LPVOID)>&& callback);
		protected:

		};
	};
}

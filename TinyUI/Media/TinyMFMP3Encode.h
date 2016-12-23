#pragma once
#include "TinyMFDecode.h"

namespace TinyUI
{
	namespace Media
	{
		/// <summary>
		/// MP3±àÂë
		/// </summary>
		class TinyMFMP3Encode : public TinyMFDecode
		{
			DISALLOW_COPY_AND_ASSIGN(TinyMFMP3Encode)
		public:
			TinyMFMP3Encode();
			virtual ~TinyMFMP3Encode();
		public:
			BOOL Open(const WAVEFORMATEX* pFMT, Callback<void(BYTE*, LONG, LPVOID)>&& callback);
		protected:

		};
	};
}

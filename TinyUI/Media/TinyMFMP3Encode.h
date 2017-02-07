#pragma once
#include "TinyMFEncode.h"

namespace TinyUI
{
	namespace Media
	{
		/// <summary>
		/// MP3±àÂë
		/// https://msdn.microsoft.com/en-us/library/windows/desktop/hh162907(v=vs.85).aspx
		/// </summary>
		class TinyMFMP3Encode : public TinyMFEncode
		{
			DISALLOW_COPY_AND_ASSIGN(TinyMFMP3Encode)
		public:
			TinyMFMP3Encode();
			virtual ~TinyMFMP3Encode();
		public:
			BOOL Open(const WAVEFORMATEX* pFMT, Callback<void(BYTE*, LONG, LPVOID)>&& callback);
		};
	};
}

#pragma once
#include "TinyMFEncode.h"

namespace TinyFramework
{
	namespace Media
	{
		/// <summary>
		/// MP3±àÂë(Window8Ö§³Ö)
		/// https://msdn.microsoft.com/en-us/library/windows/desktop/hh162907(v=vs.85).aspx
		/// CLSID_MP3ACMCodecWrapper
		/// </summary>
		class TinyMFMP3Encode : public TinyMFEncode
		{
			DISALLOW_COPY_AND_ASSIGN(TinyMFMP3Encode)
		public:
			TinyMFMP3Encode();
			virtual ~TinyMFMP3Encode();
		public:
			BOOL Open();
			BOOL SetFormat(const WAVEFORMATEX* pFMT, DWORD dwBitRate);
		};
	};
}

#pragma once
#include "TinyMFDecode.h"

namespace TinyFramework
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
			BOOL SetFormat(const MPEGLAYER3WAVEFORMAT* pMP3FMT,const WAVEFORMATEX* pFMT);
		};
	};
}

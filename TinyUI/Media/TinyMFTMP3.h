#pragma once
#include "TinyMFTDecode.h"

namespace TinyUI
{
	namespace Media
	{
		/// <summary>
		/// MP3½âÂë
		/// </summary>
		class TinyMFTMP3 : public TinyMFTDecode
		{
			DISALLOW_COPY_AND_ASSIGN(TinyMFTMP3)
		public:
			TinyMFTMP3();
			virtual ~TinyMFTMP3();
		public:
			//MPEGLAYER3WAVEFORMAT 
			BOOL Open(const WAVEFORMATEX* pFMT, Callback<void(BYTE*, LONG, LPVOID)>&& callback);
		protected:

		};
	};
}

#pragma once
#include "../Render/TinyGDI.h"
#include "TinyMFEncode.h"
#include <codecapi.h>

namespace TinyUI
{
	namespace Media
	{
		/// <summary>
		/// H264±‡¬Îƒ¨»œ ‰»ÎI420
		/// https://msdn.microsoft.com/en-us/library/windows/desktop/dd797816(v=vs.85).aspx
		/// </summary>
		class TinyMFH264Encode : public TinyMFEncode
		{
			DISALLOW_COPY_AND_ASSIGN(TinyMFH264Encode)
		public:
			TinyMFH264Encode();
			virtual ~TinyMFH264Encode();
		public:
			BOOL Open(const TinySize& size, DWORD dwFrameRate, DWORD dwBitRate, Callback<void(BYTE*, LONG, LPVOID)>&& callback);
		};
	};
}

#pragma once
#include "TinyMFEncode.h"

namespace TinyUI
{
	namespace Media
	{
#define CONST_ARRAY(name, ...) static const UINT32 name[] = { __VA_ARGS__ };
		CONST_ARRAY(VALID_BITRATES, 96000, 128000, 160000, 192000);
		CONST_ARRAY(VALID_CHANNELS, 1, 2);
		CONST_ARRAY(VALID_BITS_PER_SAMPLE, 16);
		CONST_ARRAY(VALID_SAMPLERATES, 44100, 48000);
		template <int N>
		static BOOL IsValid(const UINT32(&validValues)[N], UINT32 value)
		{
			for (UINT32 val : validValues)
			{
				if (val == value)
					return TRUE;
			}
			return FALSE;
		};
		/// <summary>
		/// AAC±àÂë(Window8Ö§³ÖADTS)
		/// https://msdn.microsoft.com/en-us/library/windows/desktop/dd742785(v=vs.85).aspx
		/// CLSID_AACMFTEncoder
		/// </summary>
		class TinyMFAACEncode : public TinyMFEncode
		{
			DISALLOW_COPY_AND_ASSIGN(TinyMFAACEncode)
		public:
			TinyMFAACEncode();
			virtual ~TinyMFAACEncode();
		public:
			BOOL SetFormat(const WAVEFORMATEX* pFMT);
		};
	};
}

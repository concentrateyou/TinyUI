#include "../stdafx.h"
#include "TinyMFMP3Decode.h"
#include <algorithm>
#include <limits>

namespace TinyFramework
{
	namespace Media
	{
		TinyMFMP3Decode::TinyMFMP3Decode()
		{
		}
		TinyMFMP3Decode::~TinyMFMP3Decode()
		{

		}
		BOOL TinyMFMP3Decode::Open()
		{
			return TinyMFDecode::Open(CLSID_CMP3DecMediaObject);
		}
		BOOL TinyMFMP3Decode::SetFormat(const MPEGLAYER3WAVEFORMAT* pMP3FMT, const WAVEFORMATEX* pFMT)
		{
			HRESULT hRes = S_OK;
			TinyComPtr<IMFMediaType> inputType;
			hRes = MFCreateMediaType(&inputType);
			if (hRes != S_OK)
				return FALSE;
			hRes = MFInitMediaTypeFromWaveFormatEx(inputType, reinterpret_cast<WAVEFORMATEX*>(const_cast<MPEGLAYER3WAVEFORMAT*>(pMP3FMT)), sizeof(MPEGLAYER3WAVEFORMAT));
			if (hRes != S_OK)
				return FALSE;
			TinyComPtr<IMFMediaType> outputType;
			hRes = MFCreateMediaType(&outputType);
			if (hRes != S_OK)
				return FALSE;
			hRes = MFInitMediaTypeFromWaveFormatEx(outputType, pFMT, sizeof(WAVEFORMATEX));
			if (hRes != S_OK)
				return FALSE;
			return TinyMFDecode::SetMediaTypes(inputType, outputType);
		}
	};
}

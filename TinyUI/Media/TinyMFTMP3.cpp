#include "../stdafx.h"
#include "TinyMFTMP3.h"
#include <algorithm>
#include <limits>

namespace TinyUI
{
	namespace Media
	{
		TinyMFTMP3::TinyMFTMP3()
		{
		}
		TinyMFTMP3::~TinyMFTMP3()
		{

		}
		BOOL TinyMFTMP3::Open(const WAVEFORMATEX* pFMT, Callback<void(BYTE*, LONG, LPVOID)>&& callback)
		{
			HRESULT hRes = S_OK;
			TinyComPtr<IMFMediaType> inputType;
			hRes = MFCreateMediaType(&inputType);
			if (FAILED(hRes))
				return FALSE;
			hRes = MFInitMediaTypeFromWaveFormatEx(inputType, pFMT, sizeof(WAVEFORMATEX) + pFMT->cbSize);
			if (FAILED(hRes))
				return FALSE;
			TinyComPtr<IMFMediaType> outputType;
			hRes = MFCreateMediaType(&outputType);
			if (FAILED(hRes))
				return FALSE;
			hRes = MFInitMediaTypeFromWaveFormatEx(outputType, pFMT, sizeof(WAVEFORMATEX) + pFMT->cbSize);
			if (FAILED(hRes))
				return FALSE;
			return TinyMFTDecode::Open(CLSID_CMP3DecMediaObject, inputType, outputType, std::move(callback));
		}
	};
}

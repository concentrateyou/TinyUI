#include "../stdafx.h"
#include "TinyMFMP3Decode.h"
#include <algorithm>
#include <limits>

namespace TinyUI
{
	namespace Media
	{
		TinyMFMP3Decode::TinyMFMP3Decode()
		{
		}
		TinyMFMP3Decode::~TinyMFMP3Decode()
		{

		}
		BOOL TinyMFMP3Decode::Open(const MPEGLAYER3WAVEFORMAT* pFMT, Callback<void(BYTE*, LONG, LPVOID)>&& callback)
		{
			HRESULT hRes = S_OK;
			TinyComPtr<IMFMediaType> inputType;
			hRes = MFCreateMediaType(&inputType);
			if (FAILED(hRes))
				return FALSE;
			hRes = MFInitMediaTypeFromWaveFormatEx(inputType, reinterpret_cast<WAVEFORMATEX*>(const_cast<MPEGLAYER3WAVEFORMAT*>(pFMT)), sizeof(MPEGLAYER3WAVEFORMAT));
			if (FAILED(hRes))
				return FALSE;
			TinyComPtr<IMFMediaType> outputType;
			hRes = MFCreateMediaType(&outputType);
			if (FAILED(hRes))
				return FALSE;
			WAVEFORMATEX sMFT = { 0 };
			sMFT.wFormatTag = WAVE_FORMAT_PCM;
			sMFT.nChannels = pFMT->wfx.nChannels;
			sMFT.nSamplesPerSec = pFMT->wfx.nSamplesPerSec;
			sMFT.wBitsPerSample = 16;
			sMFT.nBlockAlign = (sMFT.nChannels * sMFT.wBitsPerSample) / 8;
			sMFT.nAvgBytesPerSec = sMFT.nSamplesPerSec * sMFT.nBlockAlign;
			hRes = MFInitMediaTypeFromWaveFormatEx(outputType, &sMFT, sizeof(WAVEFORMATEX));
			if (FAILED(hRes))
				return FALSE;
			return TinyMFDecode::Open(CLSID_CMP3DecMediaObject, inputType, outputType, std::move(callback));
		}
	};
}

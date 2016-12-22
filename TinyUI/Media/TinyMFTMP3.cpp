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
			MPEGLAYER3WAVEFORMAT sFMT;
			ZeroMemory(&sFMT, sizeof(sFMT));
			sFMT.wID = MPEGLAYER3_ID_MPEG;
			sFMT.fdwFlags = MPEGLAYER3_FLAG_PADDING_OFF;
			sFMT.nBlockSize = WORD(144 * 128000 / 44100);
			sFMT.nFramesPerBlock = 1;
			sFMT.nCodecDelay = 0;
			sFMT.wfx.wFormatTag = WAVE_FORMAT_MPEGLAYER3;
			sFMT.wfx.nChannels = pFMT->nChannels;
			sFMT.wfx.nSamplesPerSec = pFMT->nSamplesPerSec;
			sFMT.wfx.wBitsPerSample = pFMT->wBitsPerSample;
			sFMT.wfx.nBlockAlign = (sFMT.wfx.nChannels * sFMT.wfx.wBitsPerSample) / 8;
			sFMT.wfx.nAvgBytesPerSec = sFMT.wfx.nSamplesPerSec * sFMT.wfx.nBlockAlign;
			sFMT.wfx.cbSize = MPEGLAYER3_WFX_EXTRA_BYTES;
			hRes = MFInitMediaTypeFromWaveFormatEx(inputType, (WAVEFORMATEX*)&sFMT, sizeof(WAVEFORMATEX) + sFMT.wfx.cbSize);
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

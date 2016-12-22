#include "../stdafx.h"
#include "TinyMFTAACDecode.h"
#include <algorithm>
#include <limits>

namespace TinyUI
{
	namespace Media
	{
		TinyMFTAACDecode::TinyMFTAACDecode()
		{
		}
		TinyMFTAACDecode::~TinyMFTAACDecode()
		{

		}
		BOOL TinyMFTAACDecode::Open(const WAVEFORMATEX* pFMT, Callback<void(BYTE*, LONG, LPVOID)>&& callback)
		{
			HRESULT hRes = S_OK;
			TinyComPtr<IMFMediaType> inputType;
			hRes = MFCreateMediaType(&inputType);
			if (FAILED(hRes))
				return FALSE;
			hRes = MFInitMediaTypeFromWaveFormatEx(inputType, (WAVEFORMATEX*)&pFMT, sizeof(WAVEFORMATEX) + pFMT->cbSize);
			if (FAILED(hRes))
				return FALSE;

			TinyComPtr<IMFMediaType> outputType;
			hRes = MFCreateMediaType(&outputType);
			if (FAILED(hRes))
				return FALSE;
			hRes = outputType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
			if (FAILED(hRes))
				return FALSE;
			hRes = outputType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_AAC);
			if (FAILED(hRes))
				return FALSE;
			hRes = outputType->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, pFMT->nSamplesPerSec);
			if (FAILED(hRes))
				return FALSE;
			hRes = outputType->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, pFMT->wBitsPerSample);
			if (FAILED(hRes))
				return FALSE;
			hRes = outputType->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, pFMT->nChannels);
			if (FAILED(hRes))
				return FALSE;
			hRes = outputType->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, pFMT->nAvgBytesPerSec);
			if (FAILED(hRes))
				return FALSE;
			return TinyMFTDecode::Open(CLSID_CMP3DecMediaObject, inputType, outputType, std::move(callback));
		}
	};
}

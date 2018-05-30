#include "../stdafx.h"
#include "TinyMFMP3Encode.h"
#include <algorithm>
#include <limits>

namespace TinyUI
{
	namespace Media
	{

		TinyMFMP3Encode::TinyMFMP3Encode()
		{
		}
		TinyMFMP3Encode::~TinyMFMP3Encode()
		{

		}
		BOOL TinyMFMP3Encode::SetFormat(const WAVEFORMATEX* pFMT, DWORD dwBitRate)
		{
			HRESULT hRes = S_OK;
			TinyComPtr<IMFMediaType> inputType;
			hRes = MFCreateMediaType(&inputType);
			if (hRes != S_OK)
				return FALSE;
			hRes = inputType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
			if (hRes != S_OK)
				return FALSE;
			hRes = inputType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
			if (hRes != S_OK)
				return FALSE;
			hRes = inputType->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, 16);
			if (hRes != S_OK)
				return FALSE;
			hRes = inputType->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, pFMT->nSamplesPerSec);
			if (hRes != S_OK)
				return FALSE;
			hRes = inputType->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, pFMT->nChannels);
			if (hRes != S_OK)
				return FALSE;
			TinyComPtr<IMFMediaType> outputType;
			hRes = MFCreateMediaType(&outputType);
			if (hRes != S_OK)
				return FALSE;
			hRes = outputType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
			if (hRes != S_OK)
				return FALSE;
			hRes = outputType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_MP3);
			if (hRes != S_OK)
				return FALSE;
			hRes = outputType->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, dwBitRate);// 128000/8=16000
			if (hRes != S_OK)
				return FALSE;
			hRes = outputType->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, pFMT->nChannels);
			if (hRes != S_OK)
				return FALSE;
			hRes = outputType->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, pFMT->nSamplesPerSec);
			if (hRes != S_OK)
				return FALSE;
			return TinyMFEncode::SetMediaTypes(inputType, outputType);
		}
	};
}

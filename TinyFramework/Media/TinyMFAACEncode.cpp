#include "../stdafx.h"
#include "TinyMFAACEncode.h"
#include <algorithm>
#include <limits>

namespace TinyFramework
{
	namespace Media
	{
		TinyMFAACEncode::TinyMFAACEncode()
		{
		}
		TinyMFAACEncode::~TinyMFAACEncode()
		{

		}

		BOOL TinyMFAACEncode::SetFormat(const WAVEFORMATEX* pFMT)
		{
			HRESULT hRes = S_OK;
			if (!IsValid(VALID_BITRATES, pFMT->nAvgBytesPerSec))
				return FALSE;
			if (!IsValid(VALID_BITS_PER_SAMPLE, pFMT->wBitsPerSample))
				return FALSE;
			if (!IsValid(VALID_CHANNELS, pFMT->nChannels))
				return FALSE;
			if (!IsValid(VALID_SAMPLERATES, pFMT->nSamplesPerSec))
				return FALSE;
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
			hRes = inputType->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, pFMT->wBitsPerSample);
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
			hRes = outputType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_AAC);
			if (hRes != S_OK)
				return FALSE;
			hRes = outputType->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, 16);
			if (hRes != S_OK)
				return FALSE;
			hRes = outputType->SetUINT32(MF_MT_AUDIO_BLOCK_ALIGNMENT, 1);
			if (hRes != S_OK)
				return FALSE;
			hRes = outputType->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, pFMT->nSamplesPerSec);
			if (hRes != S_OK)
				return FALSE;
			hRes = outputType->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, pFMT->nChannels);
			if (hRes != S_OK)
				return FALSE;
			hRes = outputType->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, pFMT->nAvgBytesPerSec / 8);
			if (hRes != S_OK)
				return FALSE;
			hRes = outputType->SetUINT32(MF_MT_AAC_PAYLOAD_TYPE, 1);
			if (hRes != S_OK)
				return FALSE;
			hRes = outputType->SetUINT32(MF_MT_AAC_AUDIO_PROFILE_LEVEL_INDICATION, 0x29);
			if (hRes != S_OK)
				return FALSE;
			return TinyMFEncode::SetMediaTypes(inputType, outputType);
		}
	};
}

#include "../stdafx.h"
#include "TinyMFAACEncode.h"
#include <algorithm>
#include <limits>

namespace TinyUI
{
	namespace Media
	{
		TinyMFAACEncode::TinyMFAACEncode()
		{
		}
		TinyMFAACEncode::~TinyMFAACEncode()
		{

		}
		BOOL TinyMFAACEncode::Open(const WAVEFORMATEX* pFMT, Callback<void(BYTE*, LONG, LPVOID)>&& callback)
		{
			HRESULT hRes = S_OK;
			if (pFMT->wBitsPerSample != 16 ||
				pFMT->nSamplesPerSec != 44100 ||
				pFMT->nSamplesPerSec != 48000)
			{
				return FALSE;
			}
			TinyComPtr<IMFMediaType> inputType;
			hRes = MFCreateMediaType(&inputType);
			if (FAILED(hRes))
				return FALSE;
			hRes = inputType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
			if (FAILED(hRes))
				return FALSE;
			hRes = inputType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
			if (FAILED(hRes))
				return FALSE;
			hRes = inputType->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, 16);
			if (FAILED(hRes))
				return FALSE;
			hRes = inputType->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, pFMT->nSamplesPerSec);
			if (FAILED(hRes))
				return FALSE;
			hRes = inputType->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, pFMT->nChannels);
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
			hRes = outputType->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, 16);
			if (FAILED(hRes))
				return FALSE;
			hRes = outputType->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, pFMT->nSamplesPerSec);
			if (FAILED(hRes))
				return FALSE;
			hRes = outputType->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, pFMT->nChannels);
			if (FAILED(hRes))
				return FALSE;
			hRes = outputType->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, pFMT->nAvgBytesPerSec);
			if (FAILED(hRes))
				return FALSE;
			hRes = outputType->SetUINT32(MF_MT_AAC_PAYLOAD_TYPE, 1);//ADTS
			if (FAILED(hRes))
				return FALSE;
			hRes = outputType->SetUINT32(MF_MT_AAC_AUDIO_PROFILE_LEVEL_INDICATION, 0x29);
			if (FAILED(hRes))
				return FALSE;
			return TinyMFEncode::Open(CLSID_AACMFTEncoder, inputType, outputType, std::move(callback));
		}
	};
}

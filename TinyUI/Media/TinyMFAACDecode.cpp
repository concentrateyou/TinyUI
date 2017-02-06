#include "../stdafx.h"
#include "TinyMFAACDecode.h"
#include <algorithm>
#include <limits>

namespace TinyUI
{
	namespace Media
	{
		TinyMFAACDecode::TinyMFAACDecode()
		{
		}
		TinyMFAACDecode::~TinyMFAACDecode()
		{

		}
		BOOL TinyMFAACDecode::Open(const WAVEFORMATEX* pFMT, DWORD dwBitRate, Callback<void(BYTE*, LONG, LPVOID)>&& callback)
		{
			HRESULT hRes = S_OK;
			TinyComPtr<IMFMediaType> inputType;
			hRes = MFCreateMediaType(&inputType);
			if (FAILED(hRes))
				return FALSE;
			hRes = inputType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
			if (FAILED(hRes))
				return FALSE;
			hRes = inputType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_AAC);
			if (FAILED(hRes))
				return FALSE;
			hRes = inputType->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, pFMT->wBitsPerSample);
			if (FAILED(hRes))
				return FALSE;
			hRes = inputType->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, pFMT->nSamplesPerSec);
			if (FAILED(hRes))
				return FALSE;
			hRes = inputType->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, pFMT->nChannels);
			if (FAILED(hRes))
				return FALSE;
			hRes = inputType->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, dwBitRate * 1000 / pFMT->nChannels);
			if (FAILED(hRes))
				return FALSE;
			hRes = inputType->SetUINT32(MF_MT_AUDIO_BLOCK_ALIGNMENT, pFMT->nBlockAlign);
			if (FAILED(hRes))
				return FALSE;
			hRes = inputType->SetUINT32(MF_MT_AAC_PAYLOAD_TYPE, 0x01);//ADTS
			if (FAILED(hRes))
				return FALSE;
			hRes = inputType->SetUINT32(MF_MT_AAC_AUDIO_PROFILE_LEVEL_INDICATION, 0xFE);//LOW
			if (FAILED(hRes))
				return FALSE;
			const UINT32 size = 6 * sizeof(WORD) + sizeof(DWORD);
			BYTE heaac[size];
			WORD* w = (WORD*)heaac;
			w[0] = 0x01;
			w[1] = 0xFE;
			w[2] = 0x00;
			hRes = inputType->SetBlob(MF_MT_USER_DATA, heaac, 14);
			if (FAILED(hRes))
				return FALSE;
			TinyComPtr<IMFMediaType> outputType;
			hRes = MFCreateMediaType(&outputType);
			if (FAILED(hRes))
				return FALSE;
			return TinyMFDecode::Open(CLSID_CMSAACDecMFT, inputType, outputType, std::move(callback));
		}
	};
}

#include "../stdafx.h"
#include "TinyMFMP3Encode.h"
#include <algorithm>
#include <limits>

namespace TinyUI
{
	namespace Media
	{
		typedef struct tagMPEGLAYER3
		{
			WORD         wID;
			DWORD        fdwFlags;
			WORD         wBlockSize;
			WORD         wFramesPerBlock;
			WORD         wCodecDelay;
		}MPEGLAYER3;

		TinyMFMP3Encode::TinyMFMP3Encode()
		{
		}
		TinyMFMP3Encode::~TinyMFMP3Encode()
		{

		}
		BOOL TinyMFMP3Encode::Open(const WAVEFORMATEX* pFMT, Callback<void(BYTE*, LONG, LPVOID)>&& callback)
		{
			HRESULT hRes = S_OK;
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
			hRes = outputType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_MP3);
			if (FAILED(hRes))
				return FALSE;
			hRes = outputType->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, pFMT->nAvgBytesPerSec);
			if (FAILED(hRes))
				return FALSE;
			hRes = outputType->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, pFMT->nChannels);
			if (FAILED(hRes))
				return FALSE;
			hRes = outputType->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, pFMT->nSamplesPerSec);
			if (FAILED(hRes))
				return FALSE;
			MPEGLAYER3 data;
			data.wID = MPEGLAYER3_ID_MPEG;
			data.fdwFlags = MPEGLAYER3_FLAG_PADDING_OFF;
			data.wBlockSize = WORD(144 * pFMT->nAvgBytesPerSec / pFMT->nSamplesPerSec);
			data.wFramesPerBlock = 1;
			data.wCodecDelay = 0;
			hRes = outputType->SetBlob(MF_MT_USER_DATA, reinterpret_cast<BYTE*>(&data), 14);
			if (FAILED(hRes))
				return FALSE;
			return TinyMFEncode::Open(CLSID_MP3ACMCodecWrapper, inputType, outputType, std::move(callback));
		}
	};
}

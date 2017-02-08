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
		BOOL TinyMFMP3Encode::Open(const WAVEFORMATEX* pFMT, DWORD dwBitRate, Callback<void(BYTE*, LONG, LPVOID)>&& callback)
		{
			HRESULT hRes = S_OK;

			if (pFMT->wBitsPerSample != 16)
				return FALSE;

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
			hRes = outputType->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, dwBitRate);// 128000/8=16000
			if (FAILED(hRes))
				return FALSE;
			hRes = outputType->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, pFMT->nChannels);
			if (FAILED(hRes))
				return FALSE;
			hRes = outputType->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, pFMT->nSamplesPerSec);
			if (FAILED(hRes))
				return FALSE;
			///*MPEGLAYER3WAVEFORMAT sFMT;
			//ZeroMemory(&sFMT, sizeof(sFMT));
			//sFMT.wID = MPEGLAYER3_ID_MPEG;
			//sFMT.fdwFlags = MPEGLAYER3_FLAG_PADDING_OFF;
			//sFMT.nBlockSize = WORD(144 * dwBitRate pFMT 44100);
			//sFMT.nFramesPerBlock = 1;
			//sFMT.nCodecDelay = 0;
			//sFMT.wfx.wFormatTag = WAVE_FORMAT_MPEGLAYER3;
			//sFMT.wfx.nChannels = pFMT->nChannels;
			//sFMT.wfx.nSamplesPerSec = pFMT->nSamplesPerSec;
			//sFMT.wfx.wBitsPerSample = pFMT->wBitsPerSample;
			//sFMT.wfx.nBlockAlign = (sFMT.wfx.nChannels * sFMT.wfx.wBitsPerSample) / 8;
			//sFMT.wfx.nAvgBytesPerSec = sFMT.wfx.nSamplesPerSec * sFMT.wfx.nBlockAlign;
			//sFMT.wfx.cbSize = MPEGLAYER3_WFX_EXTRA_BYTES;*/
			//MPEGLAYER3 data;
			//data.wID = MPEGLAYER3_ID_MPEG;
			//data.fdwFlags = MPEGLAYER3_FLAG_PADDING_OFF;
			//data.wBlockSize = WORD(144 * dwBitRate / pFMT->nSamplesPerSec);
			//data.wFramesPerBlock = 1;
			//data.wCodecDelay = 0;
			//hRes = outputType->SetBlob(MF_MT_USER_DATA, reinterpret_cast<BYTE*>(&data), 12);
			//if (FAILED(hRes))
			//	return FALSE;
			return TinyMFEncode::Open(CLSID_MP3ACMCodecWrapper, inputType, outputType, std::move(callback));
		}
	};
}

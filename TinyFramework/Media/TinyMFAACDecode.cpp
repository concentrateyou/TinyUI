#include "../stdafx.h"
#include "TinyMFAACDecode.h"
#include <algorithm>
#include <limits>

namespace TinyFramework
{
	namespace Media
	{
		INT GetSRIndex(UINT sampleRate)
		{
			if (92017 <= sampleRate) return 0;
			if (75132 <= sampleRate) return 1;
			if (55426 <= sampleRate) return 2;
			if (46009 <= sampleRate) return 3;
			if (37566 <= sampleRate) return 4;
			if (27713 <= sampleRate) return 5;
			if (23004 <= sampleRate) return 6;
			if (18783 <= sampleRate) return 7;
			if (13856 <= sampleRate) return 8;
			if (11502 <= sampleRate) return 9;
			if (9391 <= sampleRate) return 10;
			return 11;
		}

		TinyMFAACDecode::TinyMFAACDecode()
		{
		}
		TinyMFAACDecode::~TinyMFAACDecode()
		{

		}
		BOOL TinyMFAACDecode::Open()
		{
			return TinyMFDecode::Open(CLSID_CMSAACDecMFT);
		}
		BOOL TinyMFAACDecode::SetFormat(const BYTE* asc, LONG ascsize, const WAVEFORMATEX& waveFMT)
		{
			TinyComPtr<IMFMediaType> inputType;
			HRESULT hRes = MFCreateMediaType(&inputType);
			if (hRes != S_OK)
				return FALSE;
			hRes = inputType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
			if (hRes != S_OK)
				return FALSE;
			hRes = inputType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_AAC);
			if (hRes != S_OK)
				return FALSE;
			hRes = inputType->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, waveFMT.nSamplesPerSec);
			if (hRes != S_OK)
				return FALSE;
			hRes = inputType->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, waveFMT.nChannels);
			if (hRes != S_OK)
				return FALSE;
			hRes = inputType->SetUINT32(MF_MT_AAC_PAYLOAD_TYPE, 0x1);
			if (hRes != S_OK)
				return FALSE;
			hRes = inputType->SetUINT32(MF_MT_AAC_AUDIO_PROFILE_LEVEL_INDICATION, 0xFE);
			if (hRes != S_OK)
				return FALSE;
			const UINT32 size = 4 * sizeof(WORD) + sizeof(DWORD);
			BYTE blob[size] = { 0 };
			WORD* w = (WORD*)blob;
			w[0] = 0x1;
			w[1] = 0xFE;
			TinyScopedArray<BYTE> data(new BYTE[size + ascsize]);
			memcpy(data, blob, size);
			memcpy(data + size, asc, ascsize);
			hRes = inputType->SetBlob(MF_MT_USER_DATA, data, size + ascsize);
			if (hRes != S_OK)
				return FALSE;
			TinyComPtr<IMFMediaType> outputType;
			hRes = MFCreateMediaType(&outputType);
			if (hRes != S_OK)
				return FALSE;
			hRes = MFInitMediaTypeFromWaveFormatEx(outputType, waveFMT, sizeof(WAVEFORMATEX) + waveFMT->cbSize);
			if (hRes != S_OK)
				return FALSE;
			return TinyMFDecode::SetMediaTypes(inputType, outputType);
		}
	};
}

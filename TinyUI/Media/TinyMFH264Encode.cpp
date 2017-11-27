#include "../stdafx.h"
#include "TinyMFH264Encode.h"
#include <algorithm>
#include <limits>

namespace TinyUI
{
	namespace Media
	{
		TinyMFH264Encode::TinyMFH264Encode()
		{
		}
		TinyMFH264Encode::~TinyMFH264Encode()
		{

		}
		BOOL TinyMFH264Encode::Open(const TinySize& size, DWORD dwFrameRate, DWORD dwBitRate, Callback<void(BYTE*, LONG, LPVOID)>&& callback)
		{
			HRESULT hRes = S_OK;
			TinyComPtr<IMFMediaType> inputType;
			hRes = MFCreateMediaType(&inputType);
			if (hRes != S_OK)
				return FALSE;
			hRes = inputType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
			if (hRes != S_OK)
				return FALSE;
			hRes = inputType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_I420);
			if (hRes != S_OK)
				return FALSE;
			hRes = MFSetAttributeSize(inputType, MF_MT_FRAME_SIZE, size.cx, size.cy);
			if (hRes != S_OK)
				return FALSE;
			hRes = MFSetAttributeRatio(inputType, MF_MT_FRAME_RATE, dwFrameRate, 1);
			if (hRes != S_OK)
				return FALSE;
			hRes = inputType->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlaceMode::MFVideoInterlace_Progressive);
			if (hRes != S_OK)
				return FALSE;
			hRes = MFSetAttributeRatio(inputType, MF_MT_PIXEL_ASPECT_RATIO, 1, 1);
			if (hRes != S_OK)
				return FALSE;

			TinyComPtr<IMFMediaType> outputType;
			hRes = MFCreateMediaType(&outputType);
			if (hRes != S_OK)
				return FALSE;
			hRes = outputType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
			if (hRes != S_OK)
				return FALSE;
			hRes = outputType->SetGUID(MF_MT_MAJOR_TYPE, MFVideoFormat_H264);
			if (hRes != S_OK)
				return FALSE;
			hRes = outputType->SetUINT32(MF_MT_AVG_BITRATE, 240000);
			if (hRes != S_OK)
				return FALSE;
			hRes = MFSetAttributeSize(outputType, MF_MT_FRAME_SIZE, size.cx, size.cy);
			if (hRes != S_OK)
				return FALSE;
			hRes = MFSetAttributeRatio(outputType, MF_MT_FRAME_RATE, dwFrameRate, 1);
			if (hRes != S_OK)
				return FALSE;
			hRes = outputType->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlaceMode::MFVideoInterlace_Progressive);
			if (hRes != S_OK)
				return FALSE;
			hRes = outputType->SetUINT32(MF_MT_ALL_SAMPLES_INDEPENDENT, TRUE);
			if (hRes != S_OK)
				return FALSE;
			hRes = outputType->SetUINT32(MF_MT_MPEG2_LEVEL, (UINT32)-1);
			if (hRes != S_OK)
				return FALSE;
			hRes = outputType->SetUINT32(MF_MT_MPEG2_PROFILE, eAVEncH264VProfile_Main);
			if (hRes != S_OK)
				return FALSE;
			hRes = MFSetAttributeRatio(outputType, MF_MT_PIXEL_ASPECT_RATIO, 1, 1);
			if (hRes != S_OK)
				return FALSE;
			return TinyMFEncode::Open(CLSID_CMSH264EncoderMFT, inputType, outputType, std::move(callback));
		}
	};
}

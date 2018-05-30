#include "../stdafx.h"
#include "TinyMFIntelQSVDecode.h"
#include "../Common/TinyUtility.h"
#include <algorithm>
#include <limits>

namespace TinyUI
{
	namespace Media
	{
		EXTERN_C const GUID DECLSPEC_SELECTANY CLSID_MF_INTEL_H264DecFilter = { 0x45e5ce07, 0x5ac7, 0x4509,{ 0x94, 0xe9, 0x62, 0xdb, 0x27, 0xcf, 0x8f, 0x96 } };

		TinyMFIntelQSVDecode::TinyMFIntelQSVDecode()
		{
		}
		TinyMFIntelQSVDecode::~TinyMFIntelQSVDecode()
		{

		}
		BOOL TinyMFIntelQSVDecode::SetFormat(const TinySize& size, DWORD dwFrameRate)
		{
			HRESULT hRes = S_OK;
			TinyComPtr<IMFMediaType> inputType;
			hRes = MFCreateMediaType(&inputType);
			if (hRes != S_OK)
				return FALSE;
			hRes = inputType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
			if (hRes != S_OK)
				return FALSE;
			hRes = inputType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_H264);
			if (hRes != S_OK)
				return FALSE;
			hRes = inputType->SetUINT32(MF_MT_ALL_SAMPLES_INDEPENDENT, FALSE);
			if (hRes != S_OK)
				return FALSE;
			hRes = inputType->SetUINT32(MF_MT_FIXED_SIZE_SAMPLES, FALSE);
			if (hRes != S_OK)
				return FALSE;
			hRes = inputType->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive);
			if (hRes != S_OK)
				return FALSE;
			hRes = MFSetAttributeSize(inputType, MF_MT_FRAME_SIZE, size.cx, size.cy);
			if (hRes != S_OK)
				return FALSE;
			hRes = MFSetAttributeRatio(inputType, MF_MT_FRAME_RATE, dwFrameRate, 1);
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
			hRes = outputType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_NV12);
			if (hRes != S_OK)
				return FALSE;
			hRes = outputType->SetUINT32(MF_MT_ALL_SAMPLES_INDEPENDENT, TRUE);
			if (hRes != S_OK)
				return FALSE;
			hRes = outputType->SetUINT32(MF_MT_FIXED_SIZE_SAMPLES, TRUE);
			if (hRes != S_OK)
				return FALSE;
			hRes = outputType->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive);
			if (hRes != S_OK)
				return FALSE;
			hRes = MFSetAttributeSize(outputType, MF_MT_FRAME_SIZE, size.cx, size.cy);
			if (hRes != S_OK)
				return FALSE;
			hRes = MFSetAttributeRatio(outputType, MF_MT_FRAME_RATE, dwFrameRate, 1);
			if (hRes != S_OK)
				return FALSE;
			hRes = MFSetAttributeRatio(outputType, MF_MT_PIXEL_ASPECT_RATIO, 1, 1);
			if (hRes != S_OK)
				return FALSE;
			return TinyMFDecode::SetMediaTypes(inputType, outputType);
		}
	};
}

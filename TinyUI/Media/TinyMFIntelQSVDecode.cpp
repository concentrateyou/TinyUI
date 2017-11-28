#include "../stdafx.h"
#include "TinyMFIntelQSVDecode.h"
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
		BOOL TinyMFIntelQSVDecode::Open(Callback<void(BYTE*, LONG, LPVOID)>&& callback)
		{
			HRESULT hRes = S_OK;
			TinyComPtr<IMFMediaType> inputType;
			hRes = MFCreateMediaType(&inputType);
			if (hRes != S_OK)
				return FALSE;
			TinyComPtr<IMFMediaType> outputType;
			hRes = MFCreateMediaType(&outputType);
			if (hRes != S_OK)
				return FALSE;
			return TinyMFDecode::Open(CLSID_MF_INTEL_H264DecFilter, inputType, outputType, std::move(callback));
		}
	};
}

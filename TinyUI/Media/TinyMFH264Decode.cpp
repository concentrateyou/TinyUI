#include "../stdafx.h"
#include "TinyMFH264Decode.h"
#include <algorithm>
#include <limits>

namespace TinyUI
{
	namespace Media
	{
		TinyMFH264Decode::TinyMFH264Decode()
		{
		}
		TinyMFH264Decode::~TinyMFH264Decode()
		{

		}
		BOOL TinyMFH264Decode::SetFormat()
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
			TinyComPtr<IMFMediaType> outputType;
			hRes = MFCreateMediaType(&outputType);
			if (hRes != S_OK)
				return FALSE;
			return TinyMFDecode::SetMediaTypes(inputType, outputType);
		}
	};
}

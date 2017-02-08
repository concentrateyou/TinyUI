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
		BOOL TinyMFH264Decode::Open(Callback<void(BYTE*, LONG, LPVOID)>&& callback)
		{
			HRESULT hRes = S_OK;
			TinyComPtr<IMFMediaType> inputType;
			hRes = MFCreateMediaType(&inputType);
			if (FAILED(hRes))
				return FALSE;
			TinyComPtr<IMFMediaType> outputType;
			hRes = MFCreateMediaType(&outputType);
			if (FAILED(hRes))
				return FALSE;
			return TinyMFDecode::Open(CLSID_CMSH264DecoderMFT, inputType, outputType, std::move(callback));
		}
	};
}

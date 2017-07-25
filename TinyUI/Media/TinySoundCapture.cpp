#include "..\stdafx.h"
#include "TinySoundCapture.h"

namespace TinyUI
{
	namespace Media
	{
		TinySoundCapture::TinySoundCapture()
		{
		}

		TinySoundCapture::~TinySoundCapture()
		{
		}
		BOOL TinySoundCapture::Initialize()
		{
			HRESULT hRes = DirectSoundCaptureEnumerate(TinySoundCapture::DSEnumCallback, this);
			if (FAILED(hRes))
				return FALSE;
			return TRUE;
		}
		BOOL CALLBACK TinySoundCapture::DSEnumCallback(LPGUID, LPCSTR, LPCSTR, LPVOID)
		{
			return FALSE;
		}
	}
}
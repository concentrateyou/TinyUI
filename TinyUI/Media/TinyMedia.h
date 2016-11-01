#pragma once
#include "../Common/TinyEvent.h"
#include "../Common/TinyString.h"
#include "../Common/TinyCallback.h"
#include "../Common/TinyCore.h"
#include <mmsystem.h>
#include <dshow.h>
#include <MMDeviceAPI.h>
#include <AudioClient.h>
#include <AudioPolicy.h>
#include <avrt.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mferror.h>
#include <mfcaptureengine.h>
#include <Mftransform.h>
#include <Wmcodecdsp.h>
#include <mfplay.h>
#include <mfreadwrite.h>
#include <mmdeviceapi.h>
#include <Wmcodecdsp.h>
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "strmiids.lib")
#pragma comment(lib, "avrt.lib")
#pragma comment(lib, "Mfplat.lib")
#pragma comment(lib, "Mfuuid.lib")
#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfplay.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "wmcodecdspuuid")

namespace TinyUI
{
	namespace Media
	{
		const REFERENCE_TIME MFTIMES_PER_SEC = 10000000;
		const REFERENCE_TIME MFTIMES_PER_MS = 10000;

		class AudioObserver : public TinyLock
		{
			DECLARE_DYNAMIC(AudioObserver)
			DISALLOW_COPY_AND_ASSIGN(AudioObserver)
		public:
			AudioObserver();
			virtual void OnDataAvailable(BYTE* bits, LONG size, LPVOID lpParameter) = 0;
		protected:
			virtual ~AudioObserver();
		};
	};
}

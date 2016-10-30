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
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "strmiids.lib")
#pragma comment(lib, "avrt.lib")

namespace TinyUI
{
	namespace Media
	{
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

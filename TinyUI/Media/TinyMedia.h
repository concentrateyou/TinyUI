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
#include <functiondiscoverykeys.h>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "strmiids.lib")
#pragma comment(lib, "avrt.lib")
#pragma comment(lib, "Mfplat.lib")
#pragma comment(lib, "Mfuuid.lib")
#pragma comment(lib, "mf.lib")
#pragma comment(lib, "avrt.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfplay.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "wmcodecdspuuid.lib")

namespace TinyUI
{
	namespace Media
	{
		const REFERENCE_TIME MFTIMES_PER_SEC = 10000000;
		const REFERENCE_TIME MFTIMES_PER_MS = 10000;

#define KSAUDIO_SPEAKER_4POINT1 (KSAUDIO_SPEAKER_QUAD|SPEAKER_LOW_FREQUENCY)
#define KSAUDIO_SPEAKER_2POINT1 (KSAUDIO_SPEAKER_STEREO|SPEAKER_LOW_FREQUENCY)
#define MILLISECONDS_TO_VISUALIZE 20

#define DEFAULT_RENDER_AUDCLNT_STREAMFLAGS  (AUDCLNT_STREAMFLAGS_EVENTCALLBACK | AUDCLNT_STREAMFLAGS_NOPERSIST)
#define DEFAULT_CAPTURE_AUDCLNT_STREAMFLAGS (AUDCLNT_STREAMFLAGS_LOOPBACK | AUDCLNT_STREAMFLAGS_EVENTCALLBACK | AUDCLNT_STREAMFLAGS_NOPERSIST)

		enum ChannelLayout
		{
			CHANNEL_UNKNOWN,
			CHANNEL_MONO,
			CHANNEL_STEREO,
			CHANNEL_2POINT1,
			CHANNEL_QUAD,
			CHANNEL_4POINT1,
			CHANNEL_5POINT1,
			CHANNEL_5POINT1_SURROUND,
			CHANNEL_7POINT1,
			CHANNEL_7POINT1_SURROUND,
			CHANNEL_SURROUND,
		};

		enum AudioFormat
		{
			AUDIO_FORMAT_UNKNOWN,
			AUDIO_FORMAT_U8BIT,
			AUDIO_FORMAT_16BIT,
			AUDIO_FORMAT_32BIT,
			AUDIO_FORMAT_FLOAT,
			AUDIO_FORMAT_U8BIT_PLANAR,
			AUDIO_FORMAT_16BIT_PLANAR,
			AUDIO_FORMAT_32BIT_PLANAR,
			AUDIO_FORMAT_FLOAT_PLANAR,
		};
		static ChannelLayout ConvertChannelLayout(const WAVEFORMATEX* pFMT)
		{
			DWORD dwChannelMask = 0;
			if (pFMT->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
			{
				WAVEFORMATEXTENSIBLE *s = (WAVEFORMATEXTENSIBLE*)pFMT;
				switch (s->dwChannelMask)
				{
				case KSAUDIO_SPEAKER_QUAD:             return CHANNEL_QUAD;
				case KSAUDIO_SPEAKER_2POINT1:          return CHANNEL_2POINT1;
				case KSAUDIO_SPEAKER_4POINT1:          return CHANNEL_4POINT1;
				case KSAUDIO_SPEAKER_SURROUND:         return CHANNEL_SURROUND;
				case KSAUDIO_SPEAKER_5POINT1:          return CHANNEL_5POINT1;
				case KSAUDIO_SPEAKER_5POINT1_SURROUND: return CHANNEL_5POINT1_SURROUND;
				case KSAUDIO_SPEAKER_7POINT1:          return CHANNEL_7POINT1;
				case KSAUDIO_SPEAKER_7POINT1_SURROUND: return CHANNEL_7POINT1_SURROUND;
				}
			}
			return (ChannelLayout)pFMT->nChannels;
		}
		class TinyScopedAvrt
		{
			DISALLOW_COPY_AND_ASSIGN(TinyScopedAvrt)
		public:
			TinyScopedAvrt(LPCSTR pzTaskName);
			~TinyScopedAvrt();
			operator HANDLE() const;
			BOOL SetPriority(AVRT_PRIORITY priority = AVRT_PRIORITY_CRITICAL);
		private:
			HANDLE	m_hMM;
			DWORD	m_dwTaskIndex;
		};
		//////////////////////////////////////////////////////////////////////////
		class AudioObserver : public TinyLock
		{
			DECLARE_DYNAMIC(AudioObserver)
			DISALLOW_COPY_AND_ASSIGN(AudioObserver)
		public:
			AudioObserver();
		public:
			virtual void OnDataReceive(BYTE* bits, LONG size, LPVOID lpParameter) = 0;
		protected:
			virtual ~AudioObserver();
		};
		//////////////////////////////////////////////////////////////////////////
		class AudioDeviceListener : public TinyReference<AudioDeviceListener>, public IMMNotificationClient
		{
		public:
			HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(_In_ LPCWSTR pwstrDeviceId, _In_ DWORD dwNewState) OVERRIDE;
			HRESULT STDMETHODCALLTYPE OnDeviceAdded(_In_ LPCWSTR pwstrDeviceId) OVERRIDE;
			HRESULT STDMETHODCALLTYPE OnDeviceRemoved(_In_ LPCWSTR pwstrDeviceId);
			HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(_In_ EDataFlow flow, _In_ ERole role, _In_ LPCWSTR pwstrDefaultDeviceId) OVERRIDE;
			HRESULT STDMETHODCALLTYPE OnPropertyValueChanged(_In_ LPCWSTR pwstrDeviceId, _In_ const PROPERTYKEY key) OVERRIDE;
			HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject) OVERRIDE;
			ULONG STDMETHODCALLTYPE AddRef(void) OVERRIDE;
			ULONG STDMETHODCALLTYPE Release(void) OVERRIDE;
		};
	};
}

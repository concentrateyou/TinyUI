#pragma once
#include "../Common/TinyEvent.h"
#include "../Common/TinyString.h"
#include "../Common/TinyCallback.h"
#include "../Common/TinyCore.h"
#include "../Render/TinyGDI.h"
#include "../IO/TinyIOBuffer.h"
#include <KS.h>
#include <Codecapi.h>
#include <objbase.h>
#include <mmreg.h>
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
using namespace TinyFramework::IO;

namespace TinyFramework
{
	namespace Media
	{
#define KSAUDIO_SPEAKER_4POINT1 (KSAUDIO_SPEAKER_QUAD|SPEAKER_LOW_FREQUENCY)
#define KSAUDIO_SPEAKER_2POINT1 (KSAUDIO_SPEAKER_STEREO|SPEAKER_LOW_FREQUENCY)
#define MILLISECONDS_TO_VISUALIZE 20
#define DEFAULT_RENDER_AUDCLNT_STREAMFLAGS  (AUDCLNT_STREAMFLAGS_EVENTCALLBACK | AUDCLNT_STREAMFLAGS_NOPERSIST)
#define DEFAULT_CAPTURE_AUDCLNT_STREAMFLAGS (AUDCLNT_STREAMFLAGS_LOOPBACK | AUDCLNT_STREAMFLAGS_EVENTCALLBACK | AUDCLNT_STREAMFLAGS_NOPERSIST)
		const REFERENCE_TIME MFTIMES_PER_SEC = 10000000;
		const REFERENCE_TIME MFTIMES_PER_MS = 10000;

		enum VideoPixelFormat
		{
			PIXEL_FORMAT_UNKNOWN = 0,
			PIXEL_FORMAT_I420 = 1,
			PIXEL_FORMAT_YV12 = 2,
			PIXEL_FORMAT_YV16 = 3,
			PIXEL_FORMAT_YV12A = 4,
			PIXEL_FORMAT_YV24 = 5,
			PIXEL_FORMAT_NV12 = 6,
			PIXEL_FORMAT_NV21 = 7,
			PIXEL_FORMAT_UYVY = 8,
			PIXEL_FORMAT_YUY2 = 9,
			PIXEL_FORMAT_ARGB = 10,
			PIXEL_FORMAT_XRGB = 11,
			PIXEL_FORMAT_RGB24 = 12,
			PIXEL_FORMAT_RGB32 = 13,
			PIXEL_FORMAT_MJPEG = 14,
			PIXEL_FORMAT_MT21 = 15,
			PIXEL_FORMAT_YUV420P9 = 16,
			PIXEL_FORMAT_YUV420P10 = 17,
			PIXEL_FORMAT_YUV422P9 = 18,
			PIXEL_FORMAT_YUV422P10 = 19,
			PIXEL_FORMAT_YUV444P9 = 20,
			PIXEL_FORMAT_YUV444P10 = 21,
			PIXEL_FORMAT_MAX = PIXEL_FORMAT_YUV444P10,
		};

		enum ColorSpace
		{
			COLOR_SPACE_UNSPECIFIED = 0,
			COLOR_SPACE_JPEG = 1,
			COLOR_SPACE_HD_REC709 = 2,
			COLOR_SPACE_SD_REC601 = 3,
			COLOR_SPACE_MAX = COLOR_SPACE_SD_REC601,
		};

		enum
		{
			MaxDimension = (1 << 15) - 1,  // 32767
			MaxCanvas = (1 << (14 * 2)),  // 16384 x 16384
			MaxVideoFrames = 4,
			MaxSampleRate = 384000,
			MinSampleRate = 3000,
			MaxChannels = 32,
			MaxBytesPerSample = 4,
			MaxBitsPerSample = MaxBytesPerSample * 8,
			MaxSamplesPerPacket = MaxSampleRate,
			MaxPacketSizeInBytes = MaxBytesPerSample * MaxChannels * MaxSamplesPerPacket,
			MaxFramesPerSecond = 1000,
			MinCertificateLength = 128,
			MaxCertificateLength = 16 * 1024,
			MaxSessionIdLength = 512,
			MinKeyIdLength = 1,
			MaxKeyIdLength = 512,
			MaxKeyIds = 128,
			MaxInitDataLength = 64 * 1024,
			MaxSessionResponseLength = 64 * 1024,
			MaxKeySystemLength = 256,
		};


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
		BOOL WINAPI IsAsyncMFT(IMFTransform *pMFT, BOOL& bIsAsync);
		BOOL WINAPI UnlockAsyncMFT(IMFTransform *pMFT);
		IMFSample* WINAPI DuplicateSample(IMFSample* sapmle);

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
			DISALLOW_COPY_AND_ASSIGN(AudioObserver)
		public:
			AudioObserver();
		public:
			virtual void OnDataAvailable(BYTE* bits, LONG size, LPVOID lpParameter) = 0;
		protected:
			virtual ~AudioObserver();
		};
		//////////////////////////////////////////////////////////////////////////
		using AudioClientCallback = Callback<void(LPCWSTR, DWORD)>;

		class AudioClientListener : public TinyReference<AudioClientListener>, public IMMNotificationClient
		{
			DISALLOW_COPY_AND_ASSIGN(AudioClientListener)
		public:
			AudioClientListener();
			virtual ~AudioClientListener();
			BOOL Initialize(AudioClientCallback&& callback);
		public:
			HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(_In_ LPCWSTR pwstrDeviceId, _In_ DWORD dwNewState) OVERRIDE;
			HRESULT STDMETHODCALLTYPE OnDeviceAdded(_In_ LPCWSTR pwstrDeviceId) OVERRIDE;
			HRESULT STDMETHODCALLTYPE OnDeviceRemoved(_In_ LPCWSTR pwstrDeviceId);
			HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(_In_ EDataFlow flow, _In_ ERole role, _In_ LPCWSTR pwstrDefaultDeviceId) OVERRIDE;
			HRESULT STDMETHODCALLTYPE OnPropertyValueChanged(_In_ LPCWSTR pwstrDeviceId, _In_ const PROPERTYKEY key) OVERRIDE;
			HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject) OVERRIDE;
			ULONG STDMETHODCALLTYPE AddRef(void) OVERRIDE;
			ULONG STDMETHODCALLTYPE Release(void) OVERRIDE;
		private:
			AudioClientCallback				m_callback;
			TinyComPtr<IMMDeviceEnumerator>	m_enumerator;
		};
		//////////////////////////////////////////////////////////////////////////
		//https://msdn.microsoft.com/en-us/library/dd376684(v=vs.85).aspx
		class MediaBuffer : public TinyReference<MediaBuffer>, public IMediaBuffer
		{
		public:
			explicit MediaBuffer(DWORD dwMaxSize);
			~MediaBuffer();
		public:
			HRESULT STDMETHODCALLTYPE SetLength(DWORD cbLength) OVERRIDE;
			HRESULT STDMETHODCALLTYPE GetMaxLength(_Out_ DWORD *pcbMaxLength) OVERRIDE;
			HRESULT STDMETHODCALLTYPE GetBufferAndLength(_Outptr_opt_result_bytebuffer_(*pcbLength) BYTE **ppBuffer, _Out_opt_ DWORD *pcbLength) OVERRIDE;
			HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject) OVERRIDE;
			ULONG STDMETHODCALLTYPE AddRef(void) OVERRIDE;
			ULONG STDMETHODCALLTYPE Release(void) OVERRIDE;
		protected:
			TinyScopedArray<BYTE>	m_data;
			DWORD					m_dwSize;
			DWORD					m_dwMaxSize;
		};
		//////////////////////////////////////////////////////////////////////////
		class MFSampleQueue : public TinyReference<MFSampleQueue>, public IUnknown
		{
			class TinyNode;
		public:
			MFSampleQueue();
			virtual ~MFSampleQueue();
		public:
			HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject) OVERRIDE;
			ULONG STDMETHODCALLTYPE AddRef(void) OVERRIDE;
			ULONG STDMETHODCALLTYPE Release(void) OVERRIDE;
			BOOL	Push(IMFSample*  pSample);
			BOOL	Pop(IMFSample**  pSample);
			void	Clear(void);
			BOOL    IsEmpty(void);
			DWORD	GetSize();
		private:
			TinyLock					m_lock;
			TinyLinkList<IMFSample*>	m_list;
		};

		BOOL WINAPI GetAudioOutputType(REFCLSID clsid, IMFMediaType* inputType, const WAVEFORMATEX* pMFT, IMFMediaType** mediaType);

		typedef struct tagMediaTag
		{
			DWORD		dwType;
			DWORD		dwFlag;
			DWORD		dwTime;
			INT64		INC;
			INT64		PTS;
			INT64		DTS;
		}MediaTag;

		typedef struct tagSample
		{
			MediaTag	mediaTag;
			LONG		size;
			BYTE*		bits;
		}Sample;

		typedef struct tagSampleTag
		{
			INT64	sample;
			INT64	dts;
			INT64	pts;
			INT64	timestamp;
			INT64	duration;
			BYTE	sampleType;
			BYTE*	bits;
			LONG	size;
			INT32	linesize[8];
			INT32	x;
			INT32	y;
			INT32	cx;
			INT32	cy;
		}SampleTag;
	};
}

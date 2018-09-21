#pragma once
#include "AudioInputStream.h"
#include "AudioParameters.h"
#include "AudioPacket.h"
#include "ByteQueue.h"
#include <dsound.h>

namespace MediaSDK
{
	class WASAPIAudioInputStream : public AudioInputStream
	{
		enum AudioState
		{
			AUDIO_READY,
			AUDIO_RECORDING,
			AUDIO_STOPPING,
			AUDIO_CLOSED
		};
	public:
		WASAPIAudioInputStream();
		virtual ~WASAPIAudioInputStream();
		BOOL Initialize(const AudioParameters& params, const string& deviceID);
	public:
		BOOL Open() OVERRIDE;
		BOOL Start(AudioOutputCallback* callback) OVERRIDE;
		BOOL Stop() OVERRIDE;
		void Close() OVERRIDE;
		BOOL SetVolume(FLOAT volume) OVERRIDE;
		BOOL GetVolume(FLOAT* volume) OVERRIDE;
	private:
		void OnMessagePump();
		void HandleError(HRESULT hRes);
		BOOL FillPackage(const WAVEFORMATEX* waveFMT, UINT64 lFrequency);
		BOOL Loopback(IMMDeviceEnumerator* enumerator, const string& deviceID);
	private:
		string								m_deviceID;
		GUID								m_sessionID;
		DWORD								m_dwStreamFlag;
		UINT32								m_count;
		UINT64								m_lFrequency;
		volatile AudioState					m_state;
		TinyWorker							m_runnable;
		TinyEvent							m_sampleReady;
		TinyEvent							m_audioStop;
		AudioParameters						m_params;
		AudioOutputCallback*				m_callback;
		TinyScopedPtr<AudioPacket>			m_packet;
		TinyComPtr<IMMDevice>				m_audioDevice;
		TinyComPtr<IAudioClient>			m_audioClient;
		TinyComPtr<IAudioClient>			m_audioClientLB;
		TinyComPtr<IAudioClock>				m_audioClock;
		TinyComPtr<IAudioCaptureClient>		m_audioCapture;
		TinyComPtr<IAudioSessionControl>	m_audioSession;
		TinyComPtr<ISimpleAudioVolume>		m_audioVolume;
	};
}


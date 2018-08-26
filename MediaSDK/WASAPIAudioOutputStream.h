#pragma once
#include "AudioOutputStream.h"
#include "AudioParameters.h"
#include "AudioPacket.h"
#include "ByteQueue.h"
#include <dsound.h>

namespace MediaSDK
{
	class WASAPIAudioOutputStream : public AudioOutputStream
	{
		enum AudioState
		{
			AUDIO_READY,
			AUDIO_PLAYING,
			AUDIO_STOPPING,
			AUDIO_CLOSED
		};
	public:
		WASAPIAudioOutputStream();
		virtual ~WASAPIAudioOutputStream();
		BOOL Initialize(const AudioParameters& params, const string& deviceID, AUDCLNT_SHAREMODE mode, UINT32 engineLatency = 20);//д╛хо20ms
	public:
		BOOL Open() OVERRIDE;
		BOOL Start(AudioInputCallback* callback) OVERRIDE;
		BOOL Stop() OVERRIDE;
		BOOL GetVolume(FLOAT* volume) OVERRIDE;
		BOOL SetVolume(FLOAT volume) OVERRIDE;
		void Close() OVERRIDE;
	private:
		void OnMessagePump();
		void HandleError(HRESULT hRes);
		BOOL GetAudioClient();
		BOOL FillSilent(IAudioClient* client, IAudioRenderClient* renderClient);
		BOOL FillPackage(const WAVEFORMATEX* waveFMT, REFERENCE_TIME latency, UINT64 lFrequency);
	private:
		string							m_deviceID;
		GUID							m_sessionID;
		UINT32							m_engineLatency;
		UINT32							m_count;
		UINT64							m_writes;
		UINT64							m_lFrequency;
		REFERENCE_TIME					m_latency;
		AudioParameters					m_params;
		AUDCLNT_SHAREMODE				m_mode;
		volatile AudioState				m_state;
		TinyThread						m_runnable;
		TinyEvent						m_sampleReady;
		TinyEvent						m_audioStop;
		AudioInputCallback*				m_callback;
		TinyScopedPtr<AudioPacket>		m_packet;
		TinyComPtr<IMMDevice>			m_audioDevice;
		TinyComPtr<ISimpleAudioVolume>	m_audioVolume;
		TinyComPtr<IAudioClock>			m_audioClock;
		TinyComPtr<IAudioClient>		m_audioClient;
		TinyComPtr<IAudioRenderClient>	m_audioRenderClient;
	};
}


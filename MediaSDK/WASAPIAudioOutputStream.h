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
			AUDIO_NONE,
			AUDIO_READY,
			AUDIO_PLAYING,
			AUDIO_STOPPING,
			AUDIO_CLOSED
		};
	public:
		WASAPIAudioOutputStream();
		virtual ~WASAPIAudioOutputStream();
		BOOL Initialize(WORD wFrames, const string& deviceID, AUDCLNT_SHAREMODE mode);
	public:
		const WAVEFORMATPCMEX* GetFormat() const;
		BOOL Open() OVERRIDE;
		BOOL Start(AudioInputCallback* callback) OVERRIDE;
		BOOL Stop() OVERRIDE;
		BOOL GetVolume(DOUBLE* volume) OVERRIDE;
		BOOL SetVolume(DOUBLE volume) OVERRIDE;
		void Close() OVERRIDE;
	private:
		void OnMessagePump();
		void HandleError(HRESULT hRes);
		void FillPacket(BYTE* bits, LONG size);
		BOOL FillSilent(IAudioClient* client, IAudioRenderClient* renderClient);
		BOOL BuildFormat(REFERENCE_TIME& request);
		BOOL Render(const WAVEFORMATPCMEX& waveFMT, UINT64 lFrequency);
	private:
		WORD							m_wFrames;
		string							m_deviceID;
		GUID							m_sessionID;
		UINT32							m_count;
		UINT64							m_lFrequency;
		AUDCLNT_SHAREMODE				m_mode;
		volatile AudioState				m_state;
		WAVEFORMATPCMEX					m_waveFMT;
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


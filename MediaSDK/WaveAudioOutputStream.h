#pragma once
#include "AudioOutputStream.h"
#include "AudioParameters.h"

namespace MediaSDK
{
	class WaveAudioOutputStream : public AudioOutputStream
	{
		enum State
		{
			PCM_NONE,
			PCM_READY,
			PCM_PLAYING,
			PCM_STOPPING,
			PCM_CLOSED
		};
	public:
		WaveAudioOutputStream();
		virtual ~WaveAudioOutputStream();
	public:
		BOOL Open() OVERRIDE;
		BOOL Start(AudioInputCallback* callback) OVERRIDE;
		BOOL Stop() OVERRIDE;
		BOOL GetVolume(DOUBLE* volume) OVERRIDE;
		BOOL SetVolume(DOUBLE volume) OVERRIDE;
		void Close() OVERRIDE;
	public:
		BOOL Initialize(const AudioParameters& params, UINT count, UINT dID);
	private:
		void OutputError(MMRESULT hRes);
		void QueueNextPacket(WAVEHDR *s);
		inline WAVEHDR* GetWAVEHDR(INT n) const;
		static void NTAPI OnCallback(PVOID lpParameter, BOOLEAN timerFired);
	private:
		UINT						m_dID;
		UINT 						m_count;
		UINT						m_size;
		UINT						m_pending;
		HANDLE						m_handle;
		volatile State				m_state;
		TinyLock					m_lock;
		TinyEvent					m_event;
		TinyBuffer<CHAR>			m_buffer;
		TinyScopedPtr<AudioPacket>	m_packet;
		HWAVEOUT					m_waveO;
		WAVEFORMATPCMEX				m_waveFMT;
		AudioParameters				m_params;
		AudioInputCallback*			m_callback;
	};
}
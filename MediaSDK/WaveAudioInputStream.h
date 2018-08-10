#pragma once
#include "AudioInputStream.h"
#include "AudioParameters.h"
#include "AudioPacket.h"

namespace MediaSDK
{
	class WaveAudioInputStream : public AudioInputStream
	{
		enum State
		{
			PCM_NONE,
			PCM_READY,
			PCM_RECORDING,
			PCM_STOPPING,
			PCM_CLOSED
		};
		DISALLOW_COPY_AND_ASSIGN(WaveAudioInputStream)
	public:
		WaveAudioInputStream();
		virtual	~WaveAudioInputStream();
	public:
		BOOL Open() OVERRIDE;
		BOOL Start(AudioOutputCallback* callback) OVERRIDE;
		BOOL Stop() OVERRIDE;
		void Close() OVERRIDE;
		BOOL SetVolume(DOUBLE volume) OVERRIDE;
		BOOL GetVolume(DOUBLE* volume) OVERRIDE;
	public:
		BOOL Initialize(const AudioParameters& params, UINT count, UINT dID);
	private:
		inline WAVEHDR* GetWAVEHDR(INT index) const;
	private:
		UINT32						m_dID;
		UINT32 						m_count;//缓冲个数
		UINT32						m_size;//缓冲大小
		UINT32						m_alignsize;//缓冲大小
		WAVEFORMATEX				m_waveFMT;
		volatile State				m_state;
		AudioOutputCallback*		m_callback;
		TinyScopedPtr<AudioPacket>	m_packet;
		TinyScopedArray<CHAR>		m_bits;
	};
}



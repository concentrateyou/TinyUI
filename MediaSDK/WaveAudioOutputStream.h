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
		virtual void OnOpen();
		virtual void OnClose();
	public:
		BOOL Initialize(const AudioParameters& params, UINT count, UINT dID);
	private:
		BOOL OnDone(LPWAVEHDR pwh, DWORD_PTR dwInstance);
		void HandleError(MMRESULT hRes);
		void FillPacket(WAVEHDR *s);
		inline WAVEHDR* GetWAVEHDR(INT index) const;
		static void CALLBACK waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2);
	private:
		UINT32						m_dID;
		UINT32 						m_count;//缓冲个数
		UINT32						m_size;//缓冲大小
		UINT32						m_alignsize;//缓冲大小
		UINT32						m_pending;//硬件缓冲还没播放的数据
		volatile State				m_state;
		TinyLock					m_lock;
		TinyScopedPtr<AudioPacket>	m_packet;
		TinyScopedArray<CHAR>		m_bits;
		HWAVEOUT					m_waveO;
		WAVEFORMATPCMEX				m_waveFMT;
		AudioParameters				m_params;
		AudioInputCallback*			m_callback;
	};
}
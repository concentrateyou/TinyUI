#pragma once
#include "AudioInputStream.h"
#include "AudioParameters.h"
#include "AudioPacket.h"
#include "ByteQueue.h"
#include <dsound.h>

namespace MediaSDK
{
	class DSAudioInputStream : public AudioInputStream
	{
		enum State
		{
			PCM_READY,
			PCM_RECORDING,
			PCM_STOPPING,
			PCM_CLOSED
		};
		DISALLOW_COPY_AND_ASSIGN(DSAudioInputStream)
	public:
		DSAudioInputStream();
		virtual	~DSAudioInputStream();
	public:
		BOOL Open() OVERRIDE;
		BOOL Start(AudioOutputCallback* callback) OVERRIDE;
		BOOL Stop() OVERRIDE;
		void Close() OVERRIDE;
		BOOL SetVolume(FLOAT volume) OVERRIDE;
		BOOL GetVolume(FLOAT* volume) OVERRIDE;
	public:
		BOOL Initialize(const AudioParameters& params, UINT count, HWND hWND, const GUID& dID);
	private:
		void HandleError(HRESULT hRes);
		void OnCallback(BOOLEAN timerFired);
	private:
		GUID									m_dID;
		UINT32 									m_count;//缓冲个数
		UINT32									m_size;//缓冲大小
		DWORD									m_dwOffset;//偏移量
		WAVEFORMATEX							m_waveFMT;
		TinyEvent								m_event;
		TinyWaiter								m_waiter;
		volatile State							m_state;
		AudioParameters							m_params;
		ByteQueue								m_queue;
		AudioOutputCallback*					m_callback;
		TinyComPtr<IDirectSoundCapture8>		m_dsc8;
		TinyComPtr<IDirectSoundCaptureBuffer>	m_dscb;
		TinyComPtr<IDirectSoundCaptureBuffer8>	m_dscb8;
		TinyScopedArray<CHAR>					m_bits;
		TinyScopedPtr<AudioPacket>				m_packet;
	};
}



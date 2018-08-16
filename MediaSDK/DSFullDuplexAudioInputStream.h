#pragma once
#include "AudioInputStream.h"
#include "AudioParameters.h"
#include "AudioPacket.h"
#include "ByteQueue.h"
#include <dsound.h>

namespace MediaSDK
{
	/// <summary>
	/// 只在XP下支持
	/// </summary>
	class DSFullDuplexAudioInputStream : public AudioInputStream
	{
		enum State
		{
			PCM_NONE,
			PCM_READY,
			PCM_RECORDING,
			PCM_STOPPING,
			PCM_CLOSED
		};
		DISALLOW_COPY_AND_ASSIGN(DSFullDuplexAudioInputStream)
	public:
		DSFullDuplexAudioInputStream();
		virtual	~DSFullDuplexAudioInputStream();
	public:
		BOOL Open() OVERRIDE;
		BOOL Start(AudioOutputCallback* callback) OVERRIDE;
		BOOL Stop() OVERRIDE;
		void Close() OVERRIDE;
		BOOL SetVolume(DOUBLE volume) OVERRIDE;
		BOOL GetVolume(DOUBLE* volume) OVERRIDE;
	public:
		BOOL Initialize(const AudioParameters& params, UINT count, HWND hWND, const GUID& captureID = DSDEVID_DefaultCapture, const GUID& renderID = DSDEVID_DefaultPlayback);
	private:
		void HandleError(HRESULT hRes);
		void OnCallback(BOOLEAN timerFired);
	private:
		GUID									m_captureID;
		GUID									m_renderID;
		HWND									m_hWND;
		UINT32 									m_count;//缓冲个数
		UINT32									m_size;//缓冲大小
		DWORD									m_dwOffset;//偏移量
		WAVEFORMATEX							m_waveFMT;
		TinyEvent								m_event;
		TinyWaiter								m_waiter;
		volatile State							m_state;
		AudioParameters							m_params;
		ByteQueue								m_queue;
		TinyComPtr<IDirectSoundFullDuplex8>		m_dsfd;
		TinyComPtr<IDirectSoundBuffer8>			m_dsb8;
		TinyComPtr<IDirectSoundCaptureBuffer8>	m_dscb8;
		TinyScopedArray<CHAR>					m_bits;
		TinyScopedPtr<AudioPacket>				m_packet;
		AudioOutputCallback*					m_callback;
	};
}



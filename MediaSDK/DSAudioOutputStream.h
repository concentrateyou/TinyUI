#pragma once
#include "AudioOutputStream.h"
#include "AudioParameters.h"
#include <dsound.h>

namespace MediaSDK
{
	class DSAudioOutputStream : public AudioOutputStream
	{
		enum State
		{
			PCM_READY,
			PCM_PLAYING,
			PCM_STOPPING,
			PCM_CLOSED
		};
	private:
		struct POSITIONNOTIFY
		{
			DWORD		dwOffset;
			HANDLE		hEvent;
			HANDLE		hWaiter;
		};
	public:
		DSAudioOutputStream();
		virtual ~DSAudioOutputStream();
	public:
		BOOL Open() OVERRIDE;
		BOOL Start(AudioInputCallback* callback) OVERRIDE;
		BOOL Stop() OVERRIDE;
		BOOL GetVolume(FLOAT* volume) OVERRIDE;
		BOOL SetVolume(FLOAT volume) OVERRIDE;
		void Close() OVERRIDE;
	public:
		virtual void OnOpen();
		virtual void OnClose();
	public:
		BOOL Initialize(const AudioParameters& params, UINT count, HWND hWND, const GUID& guid);
	private:
		void OnCallback(BOOLEAN timerFired);
		void QueuePacket(CHAR* bits, UINT32 size);
		BOOL FillPacket(UINT32 offset, CHAR* bits, UINT32 size);
		void HandleError(HRESULT hRes);
	private:
		GUID								m_dID;
		HWND								m_hWND;
		UINT32 								m_count;//缓冲个数
		UINT32								m_size;//缓冲大小
		UINT32								m_packetsize;//包大小
		UINT32								m_pending;//硬件缓冲还没播放的数据
		UINT32								m_offset;//当前播放偏移量
		volatile State						m_state;
		TinyEvent							m_event;
		TinyWaiter							m_waiter;
		WAVEFORMATPCMEX						m_waveFMT;
		AudioParameters						m_params;
		AudioInputCallback*					m_callback;
		TinyScopedArray<CHAR>				m_bits;
		TinyScopedPtr<AudioPacket>			m_packet;
		TinyComPtr<IDirectSound8>			m_sound;
		TinyComPtr<IDirectSoundBuffer>		m_primary;
		TinyComPtr<IDirectSoundBuffer8>		m_secondary;
	};
}
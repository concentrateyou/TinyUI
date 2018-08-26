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
		UINT32 								m_count;//�������
		UINT32								m_size;//�����С
		UINT32								m_packetsize;//����С
		UINT32								m_pending;//Ӳ�����廹û���ŵ�����
		UINT32								m_offset;//��ǰ����ƫ����
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
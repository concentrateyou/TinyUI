#pragma once
#include "../Common/TinyCore.h"
#include "../Common/TinyEvent.h"
#include "../Common/TinyString.h"
#include "../Common/TinyCallback.h"
#include "../IO/TinyTaskBase.h"
#include "TinyWASAPIAudio.h"

namespace TinyUI
{
	namespace Media
	{
		/// <summary>
		/// 捕获声卡音频(支持AUDCLNT_STREAMFLAGS_LOOPBACK)
		/// https://msdn.microsoft.com/en-us/library/windows/desktop/dd370875(v=vs.85).aspx
		/// (eRender:音频播放设备,eCapture:音频采集设备)
		/// (eConsole:与计算机交互,eCommunications:与他人的声音交流,eMultimedia:播放或者录制电影)
		/// </summary>
		class TinyWASAPIAudioCapture : public TinyWASAPIAudio, public AudioObserver
		{
			DISALLOW_COPY_AND_ASSIGN(TinyWASAPIAudioCapture)
		public:
			TinyWASAPIAudioCapture();
			virtual ~TinyWASAPIAudioCapture();
			virtual void OnDataReceive(BYTE* bits, LONG size, LPVOID lpParameter) OVERRIDE;
		public:
			virtual	void	Initialize(Callback<void(BYTE*, LONG, LPVOID)>&& callback, DWORD dwStreamFlag = DEFAULT_CAPTURE_AUDCLNT_STREAMFLAGS);
			virtual BOOL	Open(const Name& name, WAVEFORMATEX* pFMT);
			virtual BOOL	Start();
			virtual BOOL	Stop();
			virtual BOOL	Reset();
			virtual BOOL	Close();
			virtual BOOL	SetVolume(FLOAT volume);
			virtual BOOL	GetVolume(FLOAT* volume);
			virtual BOOL	SetMute(BOOL bMute);
			virtual BOOL	GetMute(BOOL* bMute);
			WAVEFORMATEX*	GetInputFormat() const;
			BOOL			GetStreamLatency(REFERENCE_TIME& latency);
			BOOL			IsCapturing() const;
		private:
			void			OnMessagePump();
			BOOL			OpenClient(TinyComPtr<IMMDevice>& mmDevice, WAVEFORMATEX* pFMT);
		private:
			BOOL								m_bCapturing;
			DWORD								m_dwStreamFlag;
			DWORD								m_dwChannelMask;
			UINT32								m_count;
			TinyEvent							m_sampleReady;
			TinyEvent							m_audioStop;
			IO::TinyTaskBase					m_task;
			TinyScopedArray<BYTE>				m_waveFMT;
			TinyComPtr<IAudioClient>			m_audioClient;
			TinyComPtr<IAudioClient>			m_audioClientLB;
			TinyComPtr<IAudioCaptureClient>		m_audioCapture;
			TinyComPtr<IAudioSessionControl>	m_audioSession;
			TinyComPtr<ISimpleAudioVolume>		m_audioVolume;
			TinyComPtr<IChannelAudioVolume >	m_channelVolume;
			Callback<void(BYTE*, LONG, LPVOID)>	m_callback;
		};
	}
}

#pragma once
#include "../Common/TinyCore.h"
#include "../Common/TinyEvent.h"
#include "../Common/TinyString.h"
#include "../Common/TinyCallback.h"
#include "../IO/TinyTaskBase.h"
#include "TinyMedia.h"
#include "TinyWave.h"
#include <functiondiscoverykeys.h>

namespace TinyUI
{
	namespace Media
	{
		class TinyScopedAvrt
		{
			DISALLOW_COPY_AND_ASSIGN(TinyScopedAvrt);
		public:
			TinyScopedAvrt(LPCSTR pzTaskName);
			~TinyScopedAvrt();
			operator HANDLE() const;
			BOOL SetPriority(AVRT_PRIORITY priority = AVRT_PRIORITY_CRITICAL);
		private:
			HANDLE	m_hMM;
			DWORD	m_dwTaskIndex;
		};
		/// <summary>
		/// ²¶»ñÉù¿¨ÒôÆµ
		/// </summary>
		class TinyWASAPIAudioCapture : public AudioObserver
		{
		public:
			TinyWASAPIAudioCapture(DWORD dwFlag = AUDCLNT_STREAMFLAGS_LOOPBACK | AUDCLNT_STREAMFLAGS_NOPERSIST, DWORD dwLatency = 100);
			virtual ~TinyWASAPIAudioCapture();
			virtual void OnDataAvailable(BYTE* bits, LONG size, DWORD dwFlag, LPVOID lpParameter) OVERRIDE;
		public:
			virtual BOOL	Open();
			virtual BOOL	Start();
			virtual BOOL	Stop();
			virtual BOOL	Reset();
			virtual BOOL	Close();
			virtual BOOL	SetVolume(FLOAT volume);
			virtual BOOL	GetVolume(FLOAT* volume);
			virtual BOOL	SetMute(BOOL bMute);
			virtual BOOL	GetMute(BOOL* bMute);
			WAVEFORMATEX*	GetFormat() const;
			/// <summary>
			/// Á÷µÄÐ´ÈëÑÓ³Ù
			/// </summary>
			BOOL			GetStreamLatency(REFERENCE_TIME& latency);
		private:
			void			OnMessagePump();
			void			OnSampleDone(UINT32 blockAlign);
		private:
			DWORD							m_dwLatency;
			DWORD							m_dwFlag;
			TinyEvent						m_sampleReady;
			TinyEvent						m_audioStop;
			TinyScopedArray<BYTE>			m_waveEx;
			TinyComPtr<IMMDevice>			m_mmDevice;
			TinyComPtr<IAudioClient>		m_audioClient;
			TinyComPtr<IAudioClient>		m_audioClientLB;
			TinyComPtr<IAudioCaptureClient>	m_audioCapture;
			TinyComPtr<ISimpleAudioVolume>	m_audioVolume;
			IO::TinyTaskBase				m_task;
		};
	}
}

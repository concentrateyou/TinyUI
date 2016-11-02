#pragma once
#include "../Common/TinyCore.h"
#include "../Common/TinyEvent.h"
#include "../Common/TinyString.h"
#include "../Common/TinyCallback.h"
#include "../IO/TinyTaskBase.h"
#include "TinyMedia.h"
#include "TinyWave.h"
#include <functiondiscoverykeys.h>

#include "TinyMFResampler.h"

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
		/// ≤∂ªÒ…˘ø®“Ù∆µ
		/// </summary>
		class TinyWASAPIAudioCapture : public AudioObserver
		{
		public:
			TinyWASAPIAudioCapture(DWORD dwFlag = AUDCLNT_STREAMFLAGS_LOOPBACK | AUDCLNT_STREAMFLAGS_EVENTCALLBACK | AUDCLNT_STREAMFLAGS_NOPERSIST, DWORD dwLatency = 100);
			virtual ~TinyWASAPIAudioCapture();
			virtual void OnDataAvailable(BYTE* bits, LONG size, LPVOID lpParameter) OVERRIDE;
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
			void			SetCallback(Callback<void(BYTE*, LONG, LPVOID)>& callback);
			void			SetOutputFormat(const WAVEFORMATEX& ws);
			WAVEFORMATEX*	GetInputFormat() const;
			BOOL			GetStreamLatency(REFERENCE_TIME& latency);
		private:
			void			OnMessagePump();
			void			OnDataDone(UINT32 blockAlign);
		private:
			DWORD							m_dwLatency;
			DWORD							m_dwFlag;
			TinyEvent						m_sampleReady;
			TinyEvent						m_audioStop;
			TinyScopedArray<BYTE>			m_waveEx;
			WAVEFORMATEX					m_outputFormat;
			TinyMFResampler					m_resampler;
			TinyComPtr<IMMDevice>			m_mmDevice;
			TinyComPtr<IAudioClient>		m_audioClient;
			TinyComPtr<IAudioClient>		m_audioClientLB;
			TinyComPtr<IAudioCaptureClient>	m_audioCapture;
			TinyComPtr<ISimpleAudioVolume>	m_audioVolume;
			IO::TinyTaskBase				m_task;
			Callback<void(BYTE*, LONG, LPVOID)>	m_resampleCB;
			Callback<void(BYTE*, LONG, LPVOID)>	m_callback;
		};
	}
}

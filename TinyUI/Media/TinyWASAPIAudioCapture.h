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

#define MILLISECONDS_TO_VISUALIZE 20

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
			TinyWASAPIAudioCapture(DWORD dwFlag = AUDCLNT_STREAMFLAGS_LOOPBACK | AUDCLNT_STREAMFLAGS_EVENTCALLBACK | AUDCLNT_STREAMFLAGS_NOPERSIST, BOOL bUse16 = FALSE);
			virtual ~TinyWASAPIAudioCapture();
			virtual void OnDataAvailable(BYTE* bits, LONG size, LPVOID lpParameter) OVERRIDE;
		public:
			void			Initialize(Callback<void(BYTE*, LONG, LPVOID)>& callback);
			virtual BOOL	Open();
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
		private:
			void			OnMessagePump();
		private:
			BOOL										m_bUse16;
			DWORD										m_dwFlag;
			UINT32										m_bufferFrames;
			UINT32										m_captureBufferSize;
			TinyEvent									m_sampleReady;
			TinyEvent									m_audioStop;
			IO::TinyTaskBase							m_task;
			TinyScopedArray<BYTE>						m_captureBuffer;
			TinyScopedArray<BYTE>						m_waveFMT;
			TinyComPtr<IAudioClient>					m_audioClient;
			TinyComPtr<IAudioClient>					m_audioClientLB;
			TinyComPtr<IAudioCaptureClient>				m_audioCapture;
			TinyComPtr<ISimpleAudioVolume>				m_audioVolume;
			Callback<void(BYTE*, LONG, LPVOID)>	m_callback;
		};
	}
}

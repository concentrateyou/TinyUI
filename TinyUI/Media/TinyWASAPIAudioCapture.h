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
		/// <summary>
		/// ≤∂ªÒ…˘ø®“Ù∆µ
		/// </summary>
		class TinyWASAPIAudioCapture : public AudioObserver
		{
		public:
			TinyWASAPIAudioCapture(DWORD dwFlag = AUDCLNT_STREAMFLAGS_LOOPBACK | AUDCLNT_STREAMFLAGS_NOPERSIST);
			virtual ~TinyWASAPIAudioCapture();
			virtual void OnDataAvailable(BYTE* bits, LONG size, LPVOID lpParameter) OVERRIDE;
		public:
			virtual BOOL Open();
			virtual BOOL Start();
			virtual BOOL Stop();
			virtual BOOL Close();
			virtual BOOL SetVolume(FLOAT volume);
			virtual BOOL GetVolume(FLOAT* volume);
			virtual BOOL SetMute(BOOL bMute);
			virtual BOOL GetMute(BOOL* bMute);
			REFERENCE_TIME	GetLatency() const;
		private:
			void	OnMessagePump();
			void	FillSample();
		private:
			DWORD							m_dwFlag;
			UINT32							m_blockAlign;
			UINT32							m_bufferFrames;
			TinyEvent						m_samplesReady;
			TinyEvent						m_audioStop;
			TinyScopedArray<BYTE>			m_waveEx;
			TinyScopedArray<BYTE>			m_bits;
			IO::TinyTaskBase				m_task;
			TinyComPtr<IMMDevice>			m_mmDevice;
			TinyComPtr<IAudioClient>		m_audioClient;
			TinyComPtr<IAudioClient>		m_audioClientLB;
			TinyComPtr<IAudioCaptureClient>	m_audioCapture;
			TinyComPtr<ISimpleAudioVolume>	m_audioVolume;
			REFERENCE_TIME					m_latency;
			REFERENCE_TIME					m_defaultDevicePeriod;
			REFERENCE_TIME					m_minimumDevicePeriod;
		};
	}
}

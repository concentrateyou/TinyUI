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
		class TinyWASAPIAudioCapture
		{
		public:
			TinyWASAPIAudioCapture(DWORD dwFlag = AUDCLNT_STREAMFLAGS_LOOPBACK | AUDCLNT_STREAMFLAGS_NOPERSIST);
			virtual ~TinyWASAPIAudioCapture();
		public:
			virtual BOOL Open();
			virtual BOOL Start();
			virtual BOOL Stop();
			virtual BOOL SetVolume(double volume);
			virtual BOOL GetVolume(double* volume);
			virtual BOOL Close();
		private:
			void	OnMessagePump();
		private:

			Media::TinyWaveFile				m_wFile;
			TinyScopedArray<BYTE>			m_bits;
			IO::TinyTaskBase				m_task;
			TinyComPtr<IMMDevice>			m_mmDevice;
			TinyComPtr<IAudioClient>		m_audioClient;
			TinyComPtr<IAudioClient>		m_audioClientLB;
			TinyComPtr<IAudioCaptureClient>	m_audioCapture;
			TinyComPtr<ISimpleAudioVolume>	m_audioVolume;
			TinyScopedArray<BYTE>			m_captureBuffer;
			UINT							m_captureBufferSize;
			TinyEvent						m_audioSamplesReady;
			TinyEvent						m_audioStop;
			DWORD							m_dwFlag;
			UINT							m_bufferSize;
			UINT							m_bufferOffset;
			WAVEFORMATEX*					m_pWaveFormat;
			REFERENCE_TIME					m_latency;
			REFERENCE_TIME					m_defaultDevicePeriod;
			REFERENCE_TIME					m_minimumDevicePeriod;
		};
	}
}

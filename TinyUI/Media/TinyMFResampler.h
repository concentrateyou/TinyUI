#pragma once
#include "TinyMedia.h"
#include "TinyWave.h"

namespace TinyUI
{
	namespace Media
	{
		/// <summary>
		/// ÖØ²ÉÑù
		/// </summary>
		class TinyMFResampler
		{
			DISALLOW_COPY_AND_ASSIGN(TinyMFResampler);
		public:
			TinyMFResampler();
			~TinyMFResampler();
		public:
			BOOL Open(const WAVEFORMATEX* pFMTI, const WAVEFORMATEX* pFMTO, Callback<void(BYTE*, LONG, LPVOID)>& callback);
			BOOL Resample(const BYTE* bits, DWORD size);
			BOOL Close();
			virtual void OnDataAvailable(BYTE* bits, LONG size, LPVOID lpParameter);
		private:
			BOOL CreateResampler(const WAVEFORMATEX* pFMTI, const WAVEFORMATEX* pFMTO);
			BOOL CreateInputSample(const BYTE* bits, DWORD size);
			BOOL CreateOutputSample(DWORD dwSize);
			BOOL GetOutputSample(DWORD dwSize);
		private:
			TinyComPtr<IMFTransform>	m_resampler;
			TinyComPtr<IMFSample>		m_inputSample;
			TinyComPtr<IMFSample>		m_outputSample;
			WAVEFORMATEX				m_waveFMTI;
			WAVEFORMATEX				m_waveFMTO;
			Callback<void(BYTE*, LONG, LPVOID)> m_callback;
		};
	};
}

#pragma once
#include "TinyMFAPI.h"
#include "TinyWave.h"

namespace TinyUI
{
	namespace Media
	{
		/// <summary>
		/// ÖØ²ÉÑù
		/// </summary>
		class TinyResampler
		{
		public:
			TinyResampler();
			~TinyResampler();
		public:
			BOOL Open(const WAVEFORMATEX* pInputType, const WAVEFORMATEX* pOutputType);
			BOOL Resample(const BYTE* bits, DWORD size);
			BOOL Close();
			virtual void OnDataAvailable(BYTE* bits, LONG size, LPVOID lpParameter);
		private:
			BOOL CreateResampler(const WAVEFORMATEX* pInputType, const WAVEFORMATEX* pOutputType);
			BOOL CreateInputSample(const BYTE* bits, DWORD size, TinyComPtr<IMFSample>& sample);
			BOOL CreateOutputSample(TinyComPtr<IMFSample>& sample, DWORD dwSize);
			BOOL GetOutputSample(TinyComPtr<IMFSample>& sample, DWORD dwSize);
		private:
			TinyComPtr<IMFTransform>	m_resampler;
			WAVEFORMATEX				m_inputFormat;
			WAVEFORMATEX				m_outputFormat;
			TinyWaveFile				m_waveFile;
		};
	};
}

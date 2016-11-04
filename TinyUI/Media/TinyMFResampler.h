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
			BOOL Open(const WAVEFORMATEX* pInputType, const WAVEFORMATEX* pOutputType, Callback<void(BYTE*, LONG, LPVOID)>& callback);
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
			Callback<void(BYTE*, LONG, LPVOID)> m_callback;
		};
	};
}

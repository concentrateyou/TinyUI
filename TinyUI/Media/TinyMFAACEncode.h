#pragma once
#include "TinyMedia.h"
#include "TinyWave.h"

namespace TinyUI
{
	namespace Media
	{
		class TinyMFAACEncode
		{
			DISALLOW_COPY_AND_ASSIGN(TinyMFAACEncode)
		public:
			TinyMFAACEncode(DWORD dwRate = 128);
			~TinyMFAACEncode();
		public:
			BOOL Initialize(const WAVEFORMATEX* pFMTI, const WAVEFORMATEX* pFMTO, Callback<void(BYTE*, LONG, LONGLONG, LPVOID)>& callback);
			virtual void OnDataAvailable(BYTE* bits, LONG size, LONGLONG ts, LPVOID lpParameter);
		public:
			BOOL Open();
			BOOL Close();
			BOOL Encode(BYTE* bits, LONG size, LONGLONG ts);
		private:
			BOOL CreateInputSample(const BYTE* bits, DWORD size, LONGLONG ts);
			BOOL CreateOutputSample(DWORD dwSize);
			BOOL GetOutputSample(DWORD dwSize);
		private:
			UINT8						m_specificInfo[3];
			DWORD						m_dwRate;
			WAVEFORMATEX				m_waveFMTI;
			WAVEFORMATEX				m_waveFMTO;
			TinyComPtr<IMFTransform>	m_transform;
			TinyComPtr<IMFMediaType>	m_inputType;
			TinyComPtr<IMFMediaType>	m_outputType;
			TinyComPtr<IMFSample>		m_inputSample;
			TinyComPtr<IMFSample>		m_outputSample;
			Callback<void(BYTE*, LONG, LONGLONG, LPVOID)> m_callback;
		};
	};
}

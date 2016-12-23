#pragma once
#include "TinyMedia.h"
#include "TinyWave.h"

namespace TinyUI
{
	namespace Media
	{
		class TinyMFEncode
		{
			DISALLOW_COPY_AND_ASSIGN(TinyMFEncode)
		public:
			TinyMFEncode();
			virtual ~TinyMFEncode();
			virtual void OnDataAvailable(BYTE* bits, LONG size, LPVOID lpParameter);
		public:
			BOOL Open(const GUID& clsID, IMFMediaType* inputType, IMFMediaType* outputType, Callback<void(BYTE*, LONG, LPVOID)>&& callback);
			BOOL Decode(const BYTE* bits, DWORD size);
			BOOL Close();
		private:
			BOOL Create(const GUID& clsID, IMFMediaType* inputType, IMFMediaType* outputType);
			BOOL CreateInputSample(const BYTE* bits, DWORD size);
			BOOL CreateOutputSample(DWORD dwSize);
			BOOL GetOutputSample(DWORD dwSize);
		protected:
			TinyComPtr<IMFTransform>	m_transform;
			MFT_INPUT_STREAM_INFO		m_inputInfo;
			MFT_OUTPUT_STREAM_INFO		m_outputInfo;
		private:
			TinyComPtr<IMFSample>		m_inputSample;
			TinyComPtr<IMFSample>		m_outputSample;
			Callback<void(BYTE*, LONG, LPVOID)> m_callback;
		};
	};
}

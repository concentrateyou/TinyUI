#pragma once
#include "TinyMedia.h"
#include "TinyWave.h"

namespace TinyUI
{
	namespace Media
	{
		/// <summary>
		/// Win7以上版本支持
		/// </summary>
		class TinyMFEncode
		{
			DISALLOW_COPY_AND_ASSIGN(TinyMFEncode)
		public:
			TinyMFEncode();
			virtual ~TinyMFEncode();
			virtual void OnDataAvailable(BYTE* bits, LONG size, LPVOID lpParameter);
		public:
			void SetCallback(Callback<void(BYTE*, LONG, LPVOID)>&& callback);
			BOOL SetMediaTypes(IMFMediaType* inputType, IMFMediaType* outputType);
			BOOL Open(const GUID& clsID);
			BOOL Encode(const BYTE* bits, DWORD size, LONGLONG hnsSampleTime, LONGLONG hnsSampleDuration);
			BOOL Close();
			BOOL GetInputType(IMFMediaType** mediaType);
			BOOL GetOutputType(IMFMediaType** mediaType);
		private:
			BOOL CreateInputSample(const BYTE* bits, DWORD size);
			BOOL CreateOutputSample(DWORD dwSize);
			BOOL GetOutputSample(DWORD dwSize);
		protected:
			MFT_INPUT_STREAM_INFO		m_inputInfo;
			MFT_OUTPUT_STREAM_INFO		m_outputInfo;
			TinyComPtr<IMFTransform>	m_encoder;
		private:
			BOOL								m_bIsAsync;
			DWORD								m_dwInputID;
			DWORD								m_dwOutputID;
			TinyComPtr<IMFSample>				m_inputSample;
			TinyComPtr<IMFSample>				m_outputSample;
			TinyComPtr<IMFMediaEvent>			m_mediaEvent;
			TinyComPtr<IMFMediaEventGenerator>	m_eventGenerator;
			Callback<void(BYTE*, LONG, LPVOID)> m_callback;
		};
	};
}

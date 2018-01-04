#pragma once
#include "TinyMedia.h"
#include "TinyWave.h"
#include "TinyMFAsyncCallback.h"

namespace TinyUI
{
	namespace Media
	{
		/// <summary>
		/// MFT½âÂë
		/// </summary>
		class TinyMFDecode : public TinyMFAsyncCallback
		{
			DISALLOW_COPY_AND_ASSIGN(TinyMFDecode)
		public:
			TinyMFDecode();
			virtual ~TinyMFDecode();
		public:
			BOOL Open(const GUID& clsID, IMFMediaType* inputType, IMFMediaType* outputType);
			BOOL Decode(SampleTag& tag, BYTE*& bo, DWORD& so);
			BOOL Close();
			BOOL GetInputType(IMFMediaType** mediaType);
			BOOL GetOutputType(IMFMediaType** mediaType);
		private:
			BOOL Create(const GUID& clsID, IMFMediaType* inputType, IMFMediaType* outputType);
			BOOL CreateInputSample(const BYTE* bits, DWORD cbSize);
			BOOL CreateOutputSample(DWORD cbSize);
			BOOL GetOutputSample(BYTE*& bo, DWORD& so);
		protected:
			TinyComPtr<IMFTransform>		m_decoder;
		private:
			BOOL							m_bIsAsync;
			DWORD							m_dwInputID;
			DWORD							m_dwOutputID;
			DWORD							m_dwSize;
			TinyScopedArray<BYTE>			m_bits;
			TinyComPtr<IMFSample>			m_inputSample;
			TinyComPtr<IMFSample>			m_outputSample;
			TinyComPtr<IMFMediaEvent>		m_mediaEvent;
		};
	};
}

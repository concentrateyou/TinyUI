#pragma once
#include "TinyMedia.h"
#include "TinyWave.h"
#include "TinyMFAsyncCallback.h"

namespace TinyUI
{
	namespace Media
	{
		/// <summary>
		/// MFT����
		/// CLSID_CMP3DecMediaObject
		/// </summary>
		class TinyMFDecode : public TinyMFAsyncCallback
		{
			DISALLOW_COPY_AND_ASSIGN(TinyMFDecode)
		public:
			TinyMFDecode();
			virtual ~TinyMFDecode();
		public:
			BOOL SetMediaTypes(IMFMediaType* inputType, IMFMediaType* outputType);
			BOOL Open(const GUID& clsID);
			BOOL Decode(SampleTag& tag, BYTE*& bo, DWORD& so);
			BOOL Close();
			BOOL Flush();//���ģʽ��ն���
			BOOL Drain();//�ſ�ģʽ�������κ�����
			BOOL GetInputType(IMFMediaType** mediaType);
			BOOL GetOutputType(IMFMediaType** mediaType);
			void OnInvokeInput() OVERRIDE;
			void OnInvokeOutput() OVERRIDE;
		private:
			BOOL CreateInputSample(const BYTE* bits, DWORD cbSize);
			BOOL CreateOutputSample(DWORD cbSize);
			BOOL GetOutputSample(DWORD estimate, BYTE*& bo, DWORD& so);
			BOOL CreateSample(const BYTE* bits, DWORD cbSize, IMFSample** sample);
			BOOL CreateSample(DWORD cbSize, IMFSample** sample);
			BOOL GetOutputSample(DWORD estimate, IMFSample** sample);
		private:
			BOOL							m_bIsAsync;
			DWORD							m_dwInputID;
			DWORD							m_dwOutputID;
			GrowableIOBuffer				m_io;
			TinyEvent						m_events[2];
			TinyComPtr<IMFSample>			m_inputSample;
			TinyComPtr<IMFSample>			m_outputSample;
			TinyComPtr<IMFMediaEvent>		m_mediaEvent;
			TinyComPtr<IMFTransform>		m_decoder;
		};
	};
}

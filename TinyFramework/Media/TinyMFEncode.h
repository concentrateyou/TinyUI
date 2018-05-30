#pragma once
#include "TinyMedia.h"
#include "TinyWave.h"

namespace TinyFramework
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
		public:
			BOOL	IsEmpty() const;
			BOOL	SetMediaTypes(IMFMediaType* inputType, IMFMediaType* outputType);
			BOOL	Open(const GUID& clsID);
			BOOL	Encode(SampleTag& tag, BYTE*& bo, DWORD& so);
			BOOL	Close();
			BOOL	GetInputType(IMFMediaType** mediaType);
			BOOL	GetOutputType(IMFMediaType** mediaType);
		private:
			BOOL	CreateInputSample(const BYTE* bits, DWORD size);
			BOOL	CreateOutputSample(DWORD dwSize);
			BOOL	GetOutputSample(DWORD dwSize, BYTE*& bo, DWORD& so);
		protected:
			TinyComPtr<IMFTransform>			m_transform;
		private:
			BOOL								m_bIsAsync;
			DWORD								m_dwInputID;
			DWORD								m_dwOutputID;
			TinyComPtr<IMFSample>				m_inputSample;
			TinyComPtr<IMFSample>				m_outputSample;
			TinyComPtr<IMFMediaEvent>			m_mediaEvent;
			TinyComPtr<IMFMediaEventGenerator>	m_eventGenerator;
			GrowableIOBuffer					m_growableIO;
		};
	};
}

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
			TinyMFAACEncode();
			~TinyMFAACEncode();
		public:
			BOOL Initialize(const WAVEFORMATEX* pFMTI,const WAVEFORMATEX* pFMTO);
		public:
			BOOL Open();
			BOOL Close();
		private:
			TinyComPtr<IMFTransform>	m_transform;
			TinyComPtr<IMFMediaType>	m_inputType;
			TinyComPtr<IMFMediaType>	m_outputType;
		};
	};
}

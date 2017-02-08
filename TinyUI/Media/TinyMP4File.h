#pragma once
#pragma once
#include "TinyMedia.h"
#include "TinyWave.h"
#include "TinyMFEncode.h"
#include "TinyMFDecode.h"

namespace TinyUI
{
	namespace Media
	{
		class TinyMP4File
		{
		public:
			TinyMP4File();
			~TinyMP4File();
			BOOL Create(LPTSTR pzFile);
			BOOL Close();
		private:
			TinyComPtr<IMFSample>		m_sample;
			TinyComPtr<IMFSinkWriter>	m_writer;
			TinyComPtr<IMFSourceReader>	m_reader;
		};
	}
}




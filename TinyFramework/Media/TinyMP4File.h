#pragma once
#pragma once
#include "TinyMedia.h"
#include "TinyWave.h"
#include "TinyMFEncode.h"
#include "TinyMFDecode.h"

namespace TinyFramework
{
	namespace Media
	{
		class TinyMP4File
		{
			DISALLOW_COPY_AND_ASSIGN(TinyMP4File)
		public:
			TinyMP4File();
			~TinyMP4File();
			BOOL	Create(LPTSTR pzFile, IMFMediaType* inputType, IMFMediaType* outputType);
			BOOL	Open(LPTSTR pzFile);
			BOOL	Read(BYTE* lpBuffer, LONG nNumberOfBytesToRead, LPLONG lpNumberOfBytesRead, LONGLONG& timestamp);
			BOOL	Write(BYTE* lpBuffer, LONG nNumberOfBytesToRead, LONGLONG duration);
			BOOL	ResetFile();
			BOOL	Close();
		private:
			DWORD						m_dwStreamIndex;
			DWORD						m_dwMaxOutputBytes;
			LONG						m_bitRate;
			LONG						m_size;
			LONGLONG					m_sampleTime;
			TinyComPtr<IMFSample>		m_sample;
			TinyComPtr<IMFSinkWriter>	m_writer;
			TinyComPtr<IMFSourceReader>	m_reader;
		};
	}
}




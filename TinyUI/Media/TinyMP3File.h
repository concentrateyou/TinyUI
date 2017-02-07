#pragma once
#pragma once
#include "TinyMedia.h"
#include "TinyWave.h"

namespace TinyUI
{
	namespace Media
	{
		class TinyMP3File
		{
		public:
			TinyMP3File();
			~TinyMP3File();
			WAVEFORMATEX* GetFormat();
			LONG GetSize() const;
			LONG GetDuration() const;
			BOOL Create(LPTSTR pzFile, const WAVEFORMATEX* pWaveEx);
			BOOL Open(LPTSTR pzFile);
			BOOL Read(BYTE* lpBuffer, LONG nNumberOfBytesToRead, LPLONG lpNumberOfBytesRead);
			BOOL Write(BYTE* lpBuffer, LONG nNumberOfBytesToRead);
		private:
			LONG						m_size;
			LONG						m_duration;
			MPEGLAYER3WAVEFORMAT		m_sFMT;
			TinyComPtr<IMFMediaType>	m_mediaType;
			TinyComPtr<IMFSinkWriter>	m_writer;
			TinyComPtr<IMFSourceReader>	m_reader;
		};
	}
}




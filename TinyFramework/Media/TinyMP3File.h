#pragma once
#pragma once
#include "TinyMedia.h"
#include "TinyWave.h"
#include "TinyMFMP3Encode.h"
#include "TinyMFMP3Decode.h"

namespace TinyFramework
{
	namespace Media
	{
		class TinyMP3File
		{
			DISALLOW_COPY_AND_ASSIGN(TinyMP3File)
		public:
			TinyMP3File();
			~TinyMP3File();
			MPEGLAYER3WAVEFORMAT* GetFormat();
			DWORD		GetMaxOutputBytes() const;
			LONG		GetFileSize() const;
			LONG		GetBitRate() const;
			LONGLONG	GetDuration() const;
			BOOL		Create(LPCSTR pzFile, const MPEGLAYER3WAVEFORMAT* pMFT);
			BOOL		Open(LPCSTR pzFile);
			BOOL		Read(BYTE*& lpBuffer, LPLONG lpNumberOfBytesRead, LONGLONG& timestamp);
			BOOL		Write(BYTE* lpBuffer, LONG nNumberOfBytesToRead, LONGLONG duration);
			BOOL		ResetFile();
			BOOL		Close();
			static void ConvertFormat(const WAVEFORMATEX* pFMT, DWORD dwBitRate, MPEGLAYER3WAVEFORMAT* pMP3FMT);
		private:
			DWORD						m_dwStreamIndex;
			DWORD						m_dwMaxOutputBytes;
			LONG						m_bitRate;
			LONG						m_size;
			LONGLONG					m_sampleTime;
			MPEGLAYER3WAVEFORMAT		m_sFMT;
			TinyBuffer<BYTE>			m_buffer;
			TinyComPtr<IMFSample>		m_sample;
			TinyComPtr<IMFSinkWriter>	m_writer;
			TinyComPtr<IMFSourceReader>	m_reader;
		};
	}
}




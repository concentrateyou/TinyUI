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
		/// <summary>
		/// Window8 以上支持
		/// </summary>
		class TinyAACFile
		{
		public:
			TinyAACFile();
			~TinyAACFile();
			BOOL Create(LPTSTR pzFile, BOOL bADTS, const WAVEFORMATEX* pFMT);
			BOOL Read(BYTE* lpBuffer, LONG nNumberOfBytesToRead, LPLONG lpNumberOfBytesRead, LONGLONG& timestamp);
			BOOL Write(BYTE* lpBuffer, LONG nNumberOfBytesToRead);
			BOOL ResetFile();
			BOOL Close();
		private:
			static BOOL CreateAACWriter(const WAVEFORMATEX* pFMT, LPWSTR pszToFile, BOOL bADTS, IMFSinkWriter** ppSinkWriter);
		private:
			DWORD						m_dwStreamIndex;
			DWORD						m_dwMaxOutputBytes;
			LONG						m_bitRate;
			LONG						m_size;
			LONGLONG					m_duration;
			WAVEFORMATEX				m_sFMT;
			TinyComPtr<IMFSample>		m_sample;
			TinyComPtr<IMFSinkWriter>	m_writer;
			TinyComPtr<IMFSourceReader>	m_reader;
		};
	}
}




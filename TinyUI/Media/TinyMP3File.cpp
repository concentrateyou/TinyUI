#include "TinyMP3File.h"

namespace TinyUI
{
	namespace Media
	{
		TinyMP3File::TinyMP3File()
			:m_size(0),
			m_duration(0)
		{
			ZeroMemory(&m_sFMT, sizeof(m_sFMT));
		}

		TinyMP3File::~TinyMP3File()
		{
		}
		WAVEFORMATEX* TinyMP3File::GetFormat()
		{
			return reinterpret_cast<WAVEFORMATEX*>(&m_sFMT);
		}
		LONG TinyMP3File::GetSize() const
		{
			return m_size;
		}
		LONG TinyMP3File::GetDuration() const
		{
			return m_duration;
		}
		BOOL TinyMP3File::Open(LPTSTR pzFile)
		{
			if (!pzFile)
				return FALSE;
			wstring str = StringToWString(pzFile);
			HRESULT hRes = MFCreateSourceReaderFromURL(str.c_str(), NULL, &m_reader);
			if (FAILED(hRes))
				return FALSE;
			hRes = m_reader->SetStreamSelection(MF_SOURCE_READER_ALL_STREAMS, FALSE);
			if (FAILED(hRes))
				return FALSE;
			hRes = m_reader->SetStreamSelection(MF_SOURCE_READER_FIRST_AUDIO_STREAM, TRUE);
			if (FAILED(hRes))
				return FALSE;
			hRes = m_reader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, &m_mediaType);
			if (FAILED(hRes))
				return FALSE;
			UINT32 size = 0;
			WAVEFORMATEX* pFMT = NULL;
			hRes = MFCreateWaveFormatExFromMFMediaType(m_mediaType, &pFMT, &size);
			if (FAILED(hRes))
				return FALSE;
			if (sizeof(MPEGLAYER3WAVEFORMAT) != size)
			{
				CoTaskMemFree(pFMT);
				return FALSE;
			}
			memcpy(&m_sFMT, pFMT, size);
			CoTaskMemFree(pFMT);
			PROPVARIANT val;
			hRes = m_reader->GetPresentationAttribute(MF_SOURCE_READER_MEDIASOURCE, MF_PD_DURATION, &val);
			if (FAILED(hRes))
				return FALSE;
			m_duration = val.ulVal;//(val.ulVal / 10000000L);
			PropVariantClear(&val);
			hRes = m_reader->GetPresentationAttribute(MF_SOURCE_READER_MEDIASOURCE, MF_PD_TOTAL_FILE_SIZE, &val);
			if (FAILED(hRes))
				return FALSE;
			m_size = val.ulVal;
			PropVariantClear(&val);
			return TRUE;
		}
		BOOL TinyMP3File::Create(LPTSTR pzFile, const WAVEFORMATEX* pWaveEx)
		{
			if (!pzFile || !pWaveEx)
				return FALSE;
			TinyComPtr<IMFAttributes> attr;
			HRESULT hRes = MFCreateAttributes(&attr, 1);
			if (FAILED(hRes))
				return FALSE;
			hRes = attr->SetUINT32(MF_READWRITE_ENABLE_HARDWARE_TRANSFORMS, 1);
			if (FAILED(hRes))
				return FALSE;
			wstring str = StringToWString(pzFile);
			hRes = MFCreateSinkWriterFromURL(str.c_str(), NULL, attr, &m_writer);
			if (FAILED(hRes))
				return FALSE;

			return TRUE;
		}
		BOOL TinyMP3File::Read(BYTE* lpBuffer, LONG nNumberOfBytesToRead, LPLONG lpNumberOfBytesRead)
		{
			ASSERT(m_reader);
			DWORD dwStreamIndex = 0;
			DWORD dwFlags;
			LONGLONG timestamp = 0;
			TinyComPtr<IMFSample> sample;
			HRESULT hRes = m_reader->ReadSample(MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, &dwStreamIndex, &dwFlags, &timestamp, &sample);
			if (FAILED(hRes))
				return FALSE;
			if (dwFlags & MF_SOURCE_READERF_ENDOFSTREAM)
				return FALSE;
			LONG offset = 0;
			DWORD dwMaxLength = 0;
			while (offset < nNumberOfBytesToRead)
			{
				TinyComPtr<IMFMediaBuffer> mediaBuffer;
				hRes = sample->ConvertToContiguousBuffer(&mediaBuffer);
				if (FAILED(hRes))
					return FALSE;
				DWORD dwCurrentLength = 0;
				BYTE* pBits = NULL;
				hRes = mediaBuffer->Lock(&pBits, &dwMaxLength, &dwCurrentLength);
				if (FAILED(hRes))
					return FALSE;
				memcpy(lpBuffer + offset, pBits, dwCurrentLength);
				offset += dwCurrentLength;
				hRes = mediaBuffer->Unlock();
				if (FAILED(hRes))
					return FALSE;
				if ((offset + dwCurrentLength) >= nNumberOfBytesToRead)
					break;
			}
			*lpNumberOfBytesRead = offset;
			return TRUE;
		}
		BOOL TinyMP3File::Write(BYTE* lpBuffer, LONG nNumberOfBytesToRead)
		{
			ASSERT(m_writer);
			return TRUE;
		}
	}
}
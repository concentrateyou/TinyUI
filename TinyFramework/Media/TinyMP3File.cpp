#include "TinyMP3File.h"
#include <propvarutil.h>

namespace TinyFramework
{
	namespace Media
	{
		TinyMP3File::TinyMP3File()
			:m_size(0),
			m_sampleTime(0),
			m_bitRate(0),
			m_dwMaxOutputBytes(0),
			m_dwStreamIndex(MF_SOURCE_READER_FIRST_AUDIO_STREAM)
		{
			ZeroMemory(&m_sFMT, sizeof(m_sFMT));
		}

		TinyMP3File::~TinyMP3File()
		{
		}
		MPEGLAYER3WAVEFORMAT* TinyMP3File::GetFormat()
		{
			return &m_sFMT;
		}
		LONG TinyMP3File::GetFileSize() const
		{
			return m_size;
		}
		LONGLONG TinyMP3File::GetDuration() const
		{
			return m_sampleTime;
		}
		LONG TinyMP3File::GetBitRate() const
		{
			return m_bitRate;
		}
		DWORD TinyMP3File::GetMaxOutputBytes() const
		{
			return m_dwMaxOutputBytes;
		}
		BOOL TinyMP3File::Open(LPCSTR pzFile)
		{
			if (!pzFile)
				return FALSE;
			wstring str = StringToWString(pzFile);
			HRESULT hRes = MFCreateSourceReaderFromURL(str.c_str(), NULL, &m_reader);
			if (hRes != S_OK)
				return FALSE;
			hRes = m_reader->SetStreamSelection(MF_SOURCE_READER_ALL_STREAMS, FALSE);
			if (hRes != S_OK)
				return FALSE;
			hRes = m_reader->SetStreamSelection(m_dwStreamIndex, TRUE);
			if (hRes != S_OK)
				return FALSE;
			TinyComPtr<IMFMediaType> mediaType;
			hRes = m_reader->GetCurrentMediaType(m_dwStreamIndex, &mediaType);
			if (hRes != S_OK)
				return FALSE;
			UINT32 size = 0;
			WAVEFORMATEX* pFMT = NULL;
			hRes = MFCreateWaveFormatExFromMFMediaType(mediaType, &pFMT, &size);
			if (hRes != S_OK)
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
			if (hRes != S_OK)
				return FALSE;
			m_sampleTime = static_cast<LONGLONG>(val.ulVal);//(val.ulVal / 10000000L);
			PropVariantClear(&val);
			hRes = m_reader->GetPresentationAttribute(MF_SOURCE_READER_MEDIASOURCE, MF_PD_TOTAL_FILE_SIZE, &val);
			if (hRes != S_OK)
				return FALSE;
			m_size = val.ulVal;
			PropVariantClear(&val);
			hRes = m_reader->GetPresentationAttribute(MF_SOURCE_READER_MEDIASOURCE, MF_PD_AUDIO_ENCODING_BITRATE, &val);
			if (hRes != S_OK)
				return FALSE;
			m_bitRate = val.ulVal;
			PropVariantClear(&val);
			LONGLONG timestamp = 0;
			DWORD dwStreamIndex = 0;
			DWORD dwFlags;
			TinyComPtr<IMFSample> sample;
			hRes = m_reader->ReadSample(m_dwStreamIndex, 0, &dwStreamIndex, &dwFlags, &timestamp, &sample);
			if (hRes != S_OK)
				return FALSE;
			TinyComPtr<IMFMediaBuffer> mediaBuffer;
			hRes = sample->ConvertToContiguousBuffer(&mediaBuffer);
			if (hRes != S_OK)
				return FALSE;
			BYTE* pBits = NULL;
			hRes = mediaBuffer->Lock(&pBits, &m_dwMaxOutputBytes, NULL);
			if (hRes != S_OK)
				return FALSE;
			hRes = mediaBuffer->Unlock();
			if (hRes != S_OK)
				return FALSE;
			return ResetFile();
		}
		BOOL TinyMP3File::Create(LPCSTR pzFile, const MPEGLAYER3WAVEFORMAT* pMFT)
		{
			if (!pzFile || !pMFT)
				return FALSE;
			TinyComPtr<IMFAttributes> attr;
			HRESULT hRes = MFCreateAttributes(&attr, 1);
			if (hRes != S_OK)
				return FALSE;
			hRes = attr->SetUINT32(MF_READWRITE_ENABLE_HARDWARE_TRANSFORMS, 1);
			if (hRes != S_OK)
				return FALSE;
			hRes = attr->SetUINT32(MF_SINK_WRITER_DISABLE_THROTTLING, 1);
			if (hRes != S_OK)
				return FALSE;
			wstring str = StringToWString(pzFile);
			hRes = MFCreateSinkWriterFromURL(str.c_str(), NULL, attr, &m_writer);
			if (hRes != S_OK)
				return FALSE;
			TinyComPtr<IMFMediaType> mediaType;
			hRes = MFCreateMediaType(&mediaType);
			if (hRes != S_OK)
				return FALSE;
			hRes = MFInitMediaTypeFromWaveFormatEx(mediaType, reinterpret_cast<WAVEFORMATEX*>(const_cast<MPEGLAYER3WAVEFORMAT*>(pMFT)), sizeof(MPEGLAYER3WAVEFORMAT));
			if (hRes != S_OK)
				return FALSE;
			m_sFMT = *pMFT;
			hRes = MFCreateSample(&m_sample);
			if (hRes != S_OK)
				return FALSE;
			hRes = m_writer->AddStream(mediaType, &m_dwStreamIndex);
			if (hRes != S_OK)
				return FALSE;
			hRes = m_writer->BeginWriting();
			if (hRes != S_OK)
				return FALSE;
			return TRUE;
		}
		BOOL TinyMP3File::Read(BYTE*& lpBuffer, LPLONG lpNumberOfBytesRead, LONGLONG& timestamp)
		{
			ASSERT(m_reader);
			DWORD dwStreamIndex = 0;
			DWORD dwFlags;
			TinyComPtr<IMFSample> sample;
			HRESULT hRes = m_reader->ReadSample(m_dwStreamIndex, 0, &dwStreamIndex, &dwFlags, &timestamp, &sample);
			if (hRes != S_OK)
				return FALSE;
			if (dwFlags & MF_SOURCE_READERF_ENDOFSTREAM)
				return FALSE;
			TinyComPtr<IMFMediaBuffer> mediaBuffer;
			hRes = sample->ConvertToContiguousBuffer(&mediaBuffer);
			if (hRes != S_OK)
				return FALSE;
			BYTE* pBuffer = NULL;
			DWORD dwCurrentLength = 0;
			hRes = mediaBuffer->Lock(&pBuffer, NULL, &dwCurrentLength);
			if (hRes != S_OK)
				return FALSE;
			if (m_buffer.GetSize() < dwCurrentLength)
			{
				m_buffer.Reset(dwCurrentLength);
			}
			memcpy(m_buffer, pBuffer, dwCurrentLength);
			hRes = mediaBuffer->Unlock();
			if (hRes != S_OK)
				return FALSE;
			lpBuffer = m_buffer.Ptr();
			*lpNumberOfBytesRead = dwCurrentLength;
			return TRUE;
		}
		BOOL TinyMP3File::Write(BYTE* lpBuffer, LONG nNumberOfBytesToRead, LONGLONG duration)
		{
			ASSERT(m_writer);
			HRESULT hRes = m_sample->RemoveAllBuffers();
			if (hRes != S_OK)
				return FALSE;
			TinyComPtr<IMFMediaBuffer> buffer;
			hRes = MFCreateMemoryBuffer(nNumberOfBytesToRead, &buffer);
			if (hRes != S_OK)
				return FALSE;
			hRes = m_sample->AddBuffer(buffer);
			if (hRes != S_OK)
				return FALSE;
			hRes = buffer->SetCurrentLength(nNumberOfBytesToRead);
			if (hRes != S_OK)
				return FALSE;
			BYTE* pBits = NULL;
			hRes = buffer->Lock(&pBits, NULL, NULL);
			if (hRes != S_OK)
				return FALSE;
			memcpy(pBits, lpBuffer, nNumberOfBytesToRead);
			hRes = buffer->Unlock();
			if (hRes != S_OK)
				return FALSE;
			hRes = m_sample->SetSampleTime(m_sampleTime);
			if (hRes != S_OK)
				return FALSE;
			//LONGLONG duration = (10000000L * nNumberOfBytesToRead) / m_sFMT.wfx.nAvgBytesPerSec;
			hRes = m_sample->SetSampleDuration(duration);
			if (hRes != S_OK)
				return FALSE;
			hRes = m_writer->WriteSample(m_dwStreamIndex, m_sample);
			if (hRes != S_OK)
				return FALSE;
			m_sampleTime += duration;
			return TRUE;
		}
		BOOL TinyMP3File::ResetFile()
		{
			PROPVARIANT val;
			HRESULT hRes = InitPropVariantFromInt64(0, &val);
			if (hRes != S_OK)
				return FALSE;
			hRes = m_reader->SetCurrentPosition(GUID_NULL, val);
			if (hRes != S_OK)
			{
				PropVariantClear(&val);
				return FALSE;
			}
			PropVariantClear(&val);
			return TRUE;
		}
		BOOL TinyMP3File::Close()
		{
			m_sample.Release();
			if (m_writer != NULL)
			{
				m_writer->Flush(m_dwStreamIndex);
				m_writer->Finalize();
			}
			if (m_reader != NULL)
			{
				m_reader->Flush(m_dwStreamIndex);
			}
			m_reader.Release();
			return TRUE;
		}
		void TinyMP3File::ConvertFormat(const WAVEFORMATEX* pFMT, DWORD dwBitRate, MPEGLAYER3WAVEFORMAT* pMP3FMT)
		{
			ZeroMemory(pMP3FMT, sizeof(MPEGLAYER3WAVEFORMAT));
			pMP3FMT->wfx.cbSize = MPEGLAYER3_WFX_EXTRA_BYTES;
			pMP3FMT->wfx.nAvgBytesPerSec = dwBitRate / 8;
			pMP3FMT->wfx.wBitsPerSample = 0;
			pMP3FMT->wfx.nBlockAlign = 1;
			pMP3FMT->wfx.nChannels = pFMT->nChannels;
			pMP3FMT->wfx.nSamplesPerSec = pFMT->nSamplesPerSec;
			pMP3FMT->wfx.wFormatTag = WAVE_FORMAT_MPEGLAYER3;
			pMP3FMT->wID = MPEGLAYER3_ID_MPEG;
			pMP3FMT->fdwFlags = MPEGLAYER3_FLAG_PADDING_OFF;
			pMP3FMT->nBlockSize = WORD(144 * dwBitRate / pFMT->nSamplesPerSec);
			pMP3FMT->nFramesPerBlock = 1;
			pMP3FMT->nCodecDelay = 0;
		}
	}
}
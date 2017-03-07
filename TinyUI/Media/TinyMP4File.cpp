#include "TinyMP4File.h"
#include <propvarutil.h>

namespace TinyUI
{
	namespace Media
	{
		TinyMP4File::TinyMP4File()
			:m_size(0),
			m_sampleTime(0),
			m_bitRate(0),
			m_dwMaxOutputBytes(0),
			m_dwStreamIndex(MF_SOURCE_READER_FIRST_AUDIO_STREAM)
		{

		}
		TinyMP4File::~TinyMP4File()
		{

		}
		BOOL TinyMP4File::Create(LPTSTR pzFile, IMFMediaType* inputType, IMFMediaType* outputType)
		{
			if (!pzFile)
				return FALSE;
			wstring str = StringToWString(pzFile);
			TinyComPtr<IMFAttributes> attr;
			HRESULT hRes = MFCreateAttributes(&attr, 1);
			if (FAILED(hRes))
				return FALSE;
			hRes = attr->SetUINT32(MF_READWRITE_ENABLE_HARDWARE_TRANSFORMS, 1);
			if (FAILED(hRes))
				return FALSE;
			hRes = attr->SetUINT32(MF_SINK_WRITER_DISABLE_THROTTLING, 1);
			if (FAILED(hRes))
				return FALSE;
			hRes = MFCreateSinkWriterFromURL(str.c_str(), NULL, attr, &m_writer);
			if (FAILED(hRes))
				return FALSE;
			hRes = MFCreateSample(&m_sample);
			if (FAILED(hRes))
				return FALSE;
			hRes = m_writer->AddStream(outputType, &m_dwStreamIndex);
			if (FAILED(hRes))
				return FALSE;
	/*		hRes = m_writer->SetInputMediaType(m_dwStreamIndex, inputType, NULL);
			if (FAILED(hRes))
				return FALSE;*/
			hRes = m_writer->BeginWriting();
			if (FAILED(hRes))
				return FALSE;
			return TRUE;
		}
		BOOL TinyMP4File::Open(LPTSTR pzFile)
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
			hRes = m_reader->SetStreamSelection(m_dwStreamIndex, TRUE);
			if (FAILED(hRes))
				return FALSE;
			PROPVARIANT val;
			hRes = m_reader->GetPresentationAttribute(MF_SOURCE_READER_MEDIASOURCE, MF_PD_DURATION, &val);
			if (FAILED(hRes))
				return FALSE;
			m_sampleTime = static_cast<LONGLONG>(val.ulVal);//(val.ulVal / 10000000L);
			PropVariantClear(&val);
			hRes = m_reader->GetPresentationAttribute(MF_SOURCE_READER_MEDIASOURCE, MF_PD_TOTAL_FILE_SIZE, &val);
			if (FAILED(hRes))
				return FALSE;
			m_size = val.ulVal;
			PropVariantClear(&val);
			hRes = m_reader->GetPresentationAttribute(MF_SOURCE_READER_MEDIASOURCE, MF_PD_AUDIO_ENCODING_BITRATE, &val);
			if (FAILED(hRes))
				return FALSE;
			m_bitRate = val.ulVal;
			PropVariantClear(&val);
			LONGLONG timestamp = 0;
			DWORD dwStreamIndex = 0;
			DWORD dwFlags;
			TinyComPtr<IMFSample> sample;
			hRes = m_reader->ReadSample(m_dwStreamIndex, 0, &dwStreamIndex, &dwFlags, &timestamp, &sample);
			if (FAILED(hRes))
				return FALSE;
			TinyComPtr<IMFMediaBuffer> mediaBuffer;
			hRes = sample->ConvertToContiguousBuffer(&mediaBuffer);
			if (FAILED(hRes))
				return FALSE;
			BYTE* pBits = NULL;
			hRes = mediaBuffer->Lock(&pBits, &m_dwMaxOutputBytes, NULL);
			if (FAILED(hRes))
				return FALSE;
			hRes = mediaBuffer->Unlock();
			if (FAILED(hRes))
				return FALSE;
			return ResetFile();
		}
		BOOL TinyMP4File::Read(BYTE* lpBuffer, LONG nNumberOfBytesToRead, LPLONG lpNumberOfBytesRead, LONGLONG& timestamp)
		{
			ASSERT(m_reader);
			DWORD dwStreamIndex = 0;
			DWORD dwFlags;
			TinyComPtr<IMFSample> sample;
			HRESULT hRes = m_reader->ReadSample(m_dwStreamIndex, 0, &dwStreamIndex, &dwFlags, &timestamp, &sample);
			if (FAILED(hRes))
				return FALSE;
			if (dwFlags & MF_SOURCE_READERF_ENDOFSTREAM)
				return FALSE;
			TinyComPtr<IMFMediaBuffer> mediaBuffer;
			hRes = sample->ConvertToContiguousBuffer(&mediaBuffer);
			if (FAILED(hRes))
				return FALSE;
			BYTE* pBits = NULL;
			DWORD dwCurrentLength = 0;
			hRes = mediaBuffer->Lock(&pBits, NULL, &dwCurrentLength);
			if (FAILED(hRes))
				return FALSE;
			memcpy(lpBuffer, pBits, dwCurrentLength);
			hRes = mediaBuffer->Unlock();
			if (FAILED(hRes))
				return FALSE;
			*lpNumberOfBytesRead = dwCurrentLength;
			return TRUE;
		}
		BOOL TinyMP4File::Write(BYTE* lpBuffer, LONG nNumberOfBytesToRead, LONGLONG duration)
		{
			ASSERT(m_writer);
			HRESULT hRes = m_sample->RemoveAllBuffers();
			if (FAILED(hRes))
				return FALSE;
			TinyComPtr<IMFMediaBuffer> buffer;
			hRes = MFCreateMemoryBuffer(nNumberOfBytesToRead, &buffer);
			if (FAILED(hRes))
				return FALSE;
			hRes = m_sample->AddBuffer(buffer);
			if (FAILED(hRes))
				return FALSE;
			hRes = buffer->SetCurrentLength(nNumberOfBytesToRead);
			if (FAILED(hRes))
				return FALSE;
			BYTE* pBits = NULL;
			hRes = buffer->Lock(&pBits, NULL, NULL);
			if (FAILED(hRes))
				return FALSE;
			memcpy(pBits, lpBuffer, nNumberOfBytesToRead);
			hRes = buffer->Unlock();
			if (FAILED(hRes))
				return FALSE;
			hRes = m_sample->SetSampleTime(m_sampleTime);
			if (FAILED(hRes))
				return FALSE;
			//LONGLONG duration = (10000000L * nNumberOfBytesToRead) / m_dwAvgBytesPerSec;
			hRes = m_sample->SetSampleDuration(duration);
			if (FAILED(hRes))
				return FALSE;
			hRes = m_writer->WriteSample(m_dwStreamIndex, m_sample);
			if (FAILED(hRes))
				return FALSE;
			m_sampleTime += duration;
			return TRUE;
		}
		BOOL TinyMP4File::ResetFile()
		{
			PROPVARIANT val;
			HRESULT hRes = InitPropVariantFromInt64(0, &val);
			if (FAILED(hRes))
				return FALSE;
			hRes = m_reader->SetCurrentPosition(GUID_NULL, val);
			if (FAILED(hRes))
			{
				PropVariantClear(&val);
				return FALSE;
			}
			PropVariantClear(&val);
			return TRUE;
		}
		BOOL TinyMP4File::Close()
		{
			if (m_writer != NULL)
			{
				m_writer->Flush(0);
				m_writer->Finalize();
				m_writer.Release();
			}
			return TRUE;
		}
	}
}
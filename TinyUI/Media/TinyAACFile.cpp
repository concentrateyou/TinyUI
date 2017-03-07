#include "TinyAACFile.h"
#include "TinyMFAACEncode.h"
#include <propvarutil.h>

namespace TinyUI
{
	namespace Media
	{
		TinyAACFile::TinyAACFile()
			:m_size(0),
			m_duration(0),
			m_bitRate(0),
			m_dwMaxOutputBytes(0),
			m_dwStreamIndex(MF_SOURCE_READER_FIRST_AUDIO_STREAM)
		{

		}
		TinyAACFile::~TinyAACFile()
		{

		}
		BOOL TinyAACFile::CreateAACWriter(const WAVEFORMATEX* pFMT, LPWSTR pwszFileURL, BOOL bADTS, IMFSinkWriter** ppSinkWriter)
		{
			TinyComPtr<IMFByteStream> pFile;
			HRESULT hRes = MFCreateFile(MF_FILE_ACCESSMODE::MF_ACCESSMODE_READWRITE, MF_FILE_OPENMODE::MF_OPENMODE_DELETE_IF_EXIST, MF_FILE_FLAGS::MF_FILEFLAGS_NONE, pwszFileURL, &pFile);
			if (FAILED(hRes))
				return FALSE;
			TinyComPtr<IMFMediaType> pMediaType;
			hRes = MFCreateMediaType(&pMediaType);
			if (FAILED(hRes))
				return FALSE;
			pMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
			pMediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_AAC);
			pMediaType->SetUINT32(MF_MT_AAC_PAYLOAD_TYPE, bADTS ? 1 : 0);
			pMediaType->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, pFMT->nAvgBytesPerSec);
			TinyComPtr<IMFMediaSink> pAACSink;
			hRes = MFCreateADTSMediaSink(pFile, pMediaType, &pAACSink);
			if (FAILED(hRes))
				return FALSE;
			TinyComPtr<IMFAttributes> attr;
			hRes = MFCreateAttributes(&attr, 1);
			if (FAILED(hRes))
				return FALSE;
			hRes = attr->SetUINT32(MF_READWRITE_ENABLE_HARDWARE_TRANSFORMS, 1);
			if (FAILED(hRes))
				return FALSE;
			hRes = attr->SetUINT32(MF_SINK_WRITER_DISABLE_THROTTLING, 1);
			if (FAILED(hRes))
				return FALSE;
			hRes = MFCreateSinkWriterFromMediaSink(pAACSink, attr, ppSinkWriter);
			if (FAILED(hRes))
				return FALSE;
			return TRUE;
		}
		BOOL TinyAACFile::Create(LPTSTR pzFile, BOOL bADTS, const WAVEFORMATEX* pFMT)
		{
			if (!pzFile)
				return FALSE;
			if (!IsValid(VALID_BITRATES, pFMT->nAvgBytesPerSec))
				return FALSE;
			if (!IsValid(VALID_BITS_PER_SAMPLE, pFMT->wBitsPerSample))
				return FALSE;
			if (!IsValid(VALID_CHANNELS, pFMT->nChannels))
				return FALSE;
			if (!IsValid(VALID_SAMPLERATES, pFMT->nSamplesPerSec))
				return FALSE;
			wstring str = StringToWString(pzFile);
			if (!CreateAACWriter(pFMT, (LPWSTR)&str[0], bADTS, &m_writer))
				return FALSE;
			TinyComPtr<IMFMediaType> mediaType;
			HRESULT hRes = MFCreateMediaType(&mediaType);
			if (FAILED(hRes))
				return FALSE;
			hRes = mediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
			if (FAILED(hRes))
				return FALSE;
			hRes = mediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_AAC);
			if (FAILED(hRes))
				return FALSE;
			hRes = mediaType->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, 16);
			if (FAILED(hRes))
				return FALSE;
			hRes = mediaType->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, pFMT->nSamplesPerSec);
			if (FAILED(hRes))
				return FALSE;
			hRes = mediaType->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, pFMT->nChannels);
			if (FAILED(hRes))
				return FALSE;
			hRes = mediaType->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, pFMT->nAvgBytesPerSec / 8);
			if (FAILED(hRes))
				return FALSE;
			hRes = mediaType->SetUINT32(MF_MT_AAC_PAYLOAD_TYPE, 0x00);//Window8 1,0|Window7 must be 0
			if (FAILED(hRes))
				return FALSE;
			hRes = mediaType->SetUINT32(MF_MT_AAC_AUDIO_PROFILE_LEVEL_INDICATION, 0x29);
			if (FAILED(hRes))
				return FALSE;
			m_sFMT = *pFMT;
			hRes = MFCreateSample(&m_sample);
			if (FAILED(hRes))
				return FALSE;
			hRes = m_writer->AddStream(mediaType, &m_dwStreamIndex);
			if (FAILED(hRes))
				return FALSE;
			hRes = m_writer->BeginWriting();
			if (FAILED(hRes))
				return FALSE;
			return TRUE;
		}
		BOOL TinyAACFile::Read(BYTE* lpBuffer, LONG nNumberOfBytesToRead, LPLONG lpNumberOfBytesRead, LONGLONG& timestamp)
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
		BOOL TinyAACFile::Write(BYTE* lpBuffer, LONG nNumberOfBytesToRead)
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
			LONGLONG duration = (10000000L * nNumberOfBytesToRead) / m_sFMT.nAvgBytesPerSec;
			hRes = m_sample->SetSampleTime(duration);
			if (FAILED(hRes))
				return FALSE;
			hRes = m_writer->WriteSample(m_dwStreamIndex, m_sample);
			if (FAILED(hRes))
				return FALSE;
			m_duration += duration;
			return TRUE;
		}
		BOOL TinyAACFile::ResetFile()
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
		BOOL TinyAACFile::Close()
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
	}
}
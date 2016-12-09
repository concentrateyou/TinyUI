#include "../stdafx.h"
#include "TinyMFAACEncode.h"

namespace TinyUI
{
	namespace Media
	{
		TinyMFAACEncode::TinyMFAACEncode(DWORD dwRate)
			:m_dwRate(dwRate)
		{

		}
		TinyMFAACEncode::~TinyMFAACEncode()
		{

		}
		BOOL TinyMFAACEncode::Initialize(const WAVEFORMATEX* pFMTI, const WAVEFORMATEX* pFMTO, Callback<void(BYTE*, LONG, LONGLONG, LPVOID)>&& callback)
		{
			m_callback = std::move(callback);
			m_waveFMTI = *pFMTI;
			m_waveFMTO = *pFMTO;
			TinyComPtr<IUnknown> unknow;
			HRESULT hRes = unknow.CoCreateInstance(CLSID_AACMFTEncoder, NULL, CLSCTX_INPROC_SERVER);
			if (FAILED(hRes))
				return FALSE;
			hRes = unknow->QueryInterface(IID_PPV_ARGS(&m_transform));
			if (FAILED(hRes))
				return FALSE;
			hRes = MFCreateMediaType(&m_inputType);
			if (FAILED(hRes))
				return FALSE;
			hRes = MFCreateMediaType(&m_outputType);
			if (FAILED(hRes))
				return FALSE;
			hRes = m_outputType->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, (m_dwRate * 1000) / 8);
			if (FAILED(hRes))
				return FALSE;
			hRes = m_transform->SetInputType(0, m_inputType, 0);
			if (FAILED(hRes))
				return FALSE;
			hRes = m_transform->SetOutputType(0, m_outputType, 0);
			if (FAILED(hRes))
				return FALSE;
			return TRUE;
		}
		void TinyMFAACEncode::OnDataAvailable(BYTE* bits, LONG size, LONGLONG ts, LPVOID lpParameter)
		{
			if (!m_callback.IsNull())
			{
				m_callback(bits, size, ts, lpParameter);
			}
		}
		BOOL TinyMFAACEncode::Open()
		{
			HRESULT hRes = m_transform->ProcessMessage(MFT_MESSAGE_NOTIFY_BEGIN_STREAMING, NULL);
			if (FAILED(hRes))
				return FALSE;
			hRes = m_transform->ProcessMessage(MFT_MESSAGE_NOTIFY_START_OF_STREAM, NULL);
			if (FAILED(hRes))
				return FALSE;
			return TRUE;
		}
		BOOL TinyMFAACEncode::Close()
		{
			HRESULT hRes = m_transform->ProcessMessage(MFT_MESSAGE_NOTIFY_END_STREAMING, NULL);
			if (FAILED(hRes))
				return FALSE;
			hRes = m_transform->ProcessMessage(MFT_MESSAGE_NOTIFY_END_OF_STREAM, NULL);
			if (FAILED(hRes))
				return FALSE;
			return TRUE;
		}
		BOOL TinyMFAACEncode::Encode(BYTE* bits, LONG size, LONGLONG ts)
		{
			HRESULT hRes = S_OK;
			if (!CreateInputSample(bits, size, ts))
				return FALSE;
			hRes = m_transform->ProcessInput(0, m_inputSample, 0);
			if (FAILED(hRes))
			{
				if (hRes == MF_E_NOTACCEPTING)
				{
					//TODO;
				}
				return FALSE;
			}
			DWORD dwFlag = 0;
			hRes = m_transform->GetOutputStatus(&dwFlag);
			if (dwFlag != MFT_OUTPUT_STATUS_SAMPLE_READY)//需要更多地采样
			{
				return FALSE;
			}
			MFT_OUTPUT_STREAM_INFO info = { 0 };
			hRes = m_transform->GetOutputStreamInfo(0, &info);
			if (FAILED(hRes))
				return FALSE;
			return GetOutputSample(info.cbSize);
		}
		BOOL TinyMFAACEncode::CreateInputSample(const BYTE* bits, DWORD size, LONGLONG ts)
		{
			HRESULT hRes = S_OK;
			TinyComPtr<IMFMediaBuffer> buffer;
			if (!m_inputSample)
			{
				hRes = MFCreateSample(&m_inputSample);
				if (FAILED(hRes))
					return FALSE;
				hRes = MFCreateMemoryBuffer(size, &buffer);
				if (FAILED(hRes))
					return FALSE;
			}
			hRes = m_inputSample->GetBufferByIndex(0, &buffer);
			if (FAILED(hRes))
				return FALSE;
			DWORD dwMax = 0;
			hRes = buffer->GetMaxLength(&dwMax);
			if (FAILED(hRes))
				return FALSE;
			if (dwMax < size)
			{
				hRes = m_inputSample->RemoveAllBuffers();
				if (FAILED(hRes))
					return FALSE;
				hRes = MFCreateMemoryBuffer(size, &buffer);
				if (FAILED(hRes))
					return FALSE;
				hRes = m_inputSample->AddBuffer(buffer);
				if (FAILED(hRes))
					return FALSE;
			}
			BYTE* ps = NULL;
			hRes = buffer->Lock(&ps, NULL, NULL);
			if (FAILED(hRes))
				return FALSE;
			memcpy(ps, bits, size);
			hRes = buffer->Unlock();
			if (FAILED(hRes))
				return FALSE;
			hRes = buffer->SetCurrentLength(size);
			if (FAILED(hRes))
				return FALSE;
			hRes = m_inputSample->SetSampleTime(ts);
			if (FAILED(hRes))
				return FALSE;
			LONGLONG hnsSampleDuration = (LONGLONG)(((FLOAT)m_waveFMTI.nSamplesPerSec / m_waveFMTI.nChannels / (size / m_waveFMTI.nBlockAlign)) * 10000);
			hRes = m_inputSample->SetSampleDuration(hnsSampleDuration);
			if (FAILED(hRes))
				return FALSE;
			return TRUE;
		}
		BOOL TinyMFAACEncode::CreateOutputSample(DWORD dwSize)
		{
			HRESULT hRes = S_OK;
			TinyComPtr<IMFMediaBuffer> buffer;
			if (!m_outputSample)
			{
				hRes = MFCreateSample(&m_outputSample);
				if (FAILED(hRes))
					return FALSE;
				hRes = MFCreateMemoryBuffer(dwSize, &buffer);
				if (FAILED(hRes))
					return FALSE;
				hRes = m_outputSample->AddBuffer(buffer);
				if (FAILED(hRes))
					return FALSE;
			}
			hRes = m_outputSample->GetBufferByIndex(0, &buffer);
			if (FAILED(hRes))
				return FALSE;
			DWORD dwMax = 0;
			hRes = buffer->GetMaxLength(&dwMax);
			if (FAILED(hRes))
				return FALSE;
			if (dwMax < dwSize)
			{
				hRes = m_outputSample->RemoveAllBuffers();
				if (FAILED(hRes))
					return FALSE;
				hRes = MFCreateMemoryBuffer(dwSize, &buffer);
				if (FAILED(hRes))
					return FALSE;
				hRes = m_outputSample->AddBuffer(buffer);
				if (FAILED(hRes))
					return FALSE;
			}
			return TRUE;
		}
		BOOL TinyMFAACEncode::GetOutputSample(DWORD dwSize)
		{
			ASSERT(m_transform);
			HRESULT hRes = S_OK;
			TinyBufferArray<BYTE>	sampleBuffer;
			for (;;)
			{
				MFT_OUTPUT_DATA_BUFFER samples = { 0 };
				if (!CreateOutputSample(dwSize))
					return FALSE;
				samples.pSample = m_outputSample;
				DWORD dwStatus;
				hRes = m_transform->ProcessOutput(0, 1, &samples, &dwStatus);
				if (hRes != S_OK)
				{
					if (hRes == MF_E_TRANSFORM_NEED_MORE_INPUT)
					{
						break;
					}
					return FALSE;
				}
				TinyComPtr<IMFMediaBuffer> buffer;
				hRes = samples.pSample->ConvertToContiguousBuffer(&buffer);
				if (hRes != S_OK)
					return FALSE;
				DWORD dwCurrentSize = 0;
				hRes = buffer->GetCurrentLength(&dwCurrentSize);
				if (hRes != S_OK)
					return FALSE;
				BYTE* pBuffer = NULL;
				hRes = buffer->Lock(&pBuffer, NULL, NULL);
				if (hRes != S_OK)
					return FALSE;
				sampleBuffer.Add(pBuffer, dwCurrentSize);
				hRes = buffer->Unlock();
				if (hRes != S_OK)
					return FALSE;
			}
			LONGLONG ts = 0;
			hRes = m_outputSample->GetSampleTime(&ts);
			if (hRes != S_OK)
				return FALSE;
			OnDataAvailable(sampleBuffer.m_value, sampleBuffer.m_size, ts, this);
			return TRUE;
		}
	};
}

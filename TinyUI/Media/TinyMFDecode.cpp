#include "../stdafx.h"
#include "TinyMFDecode.h"
#include <algorithm>
#include <limits>

namespace TinyUI
{
	namespace Media
	{
		TinyMFDecode::TinyMFDecode()
			:m_bIsAsync(FALSE),
			m_dwInputID(0),
			m_dwOutputID(0)
		{

		}
		TinyMFDecode::~TinyMFDecode()
		{
			Close();
		}
		BOOL TinyMFDecode::GetInputType(IMFMediaType** mediaType)
		{
			ASSERT(m_decoder);
			return SUCCEEDED(m_decoder->GetInputCurrentType(m_dwInputID, mediaType));
		}
		BOOL TinyMFDecode::GetOutputType(IMFMediaType** mediaType)
		{
			ASSERT(m_decoder);
			return SUCCEEDED(m_decoder->GetOutputCurrentType(m_dwOutputID, mediaType));
		}
		BOOL TinyMFDecode::Create(const GUID& clsID, IMFMediaType* inputType, IMFMediaType* outputType)
		{
			HRESULT hRes = S_OK;
			TinyComPtr<IUnknown> unknow;
			hRes = unknow.CoCreateInstance(clsID, NULL, CLSCTX_INPROC_SERVER);
			if (hRes != S_OK)
				return FALSE;
			hRes = unknow->QueryInterface(IID_PPV_ARGS(&m_decoder));
			if (hRes != S_OK)
				return FALSE;
			DWORD dwInputStreams = 0;
			DWORD dwOutputSample = 0;
			hRes = m_decoder->GetStreamCount(&dwInputStreams, &dwOutputSample);
			if (hRes != S_OK)
				return FALSE;
			if (dwInputStreams == 0 || dwOutputSample == 0)
				return FALSE;
			TinyScopedArray<DWORD> dwInputIDs;
			TinyScopedArray<DWORD> dwOutputIDs;
			hRes = m_decoder->GetStreamIDs(dwInputStreams, dwInputIDs, dwOutputSample, dwOutputIDs);
			if (hRes == MF_E_TRANSFORM_ASYNC_LOCKED)
			{
				UnlockAsyncMFT(m_decoder);
				hRes = m_decoder->GetStreamIDs(dwInputStreams, dwInputIDs, dwOutputSample, dwOutputIDs);
			}
			if (hRes == S_OK)
			{
				m_dwInputID = dwInputIDs[0];
				m_dwOutputID = dwOutputIDs[0];
			}
			if (inputType != NULL)
			{
				hRes = m_decoder->SetInputType(m_dwInputID, inputType, 0);
				if (hRes != S_OK)
					return FALSE;
				if (hRes != S_OK)
					return FALSE;
				if (outputType == NULL)
				{
					TinyComPtr<IMFMediaType> mediaType;
					DWORD dwTypeIndex = 0;
					while (SUCCEEDED(m_decoder->GetOutputAvailableType(m_dwOutputID, dwTypeIndex++, &mediaType)))
					{
						BOOL bResult;
						hRes = mediaType->Compare(mediaType, MF_ATTRIBUTES_MATCH_OUR_ITEMS, &bResult);
						if (SUCCEEDED(hRes) && bResult)
						{
							hRes = m_decoder->SetOutputType(m_dwOutputID, mediaType, 0);
							if (hRes != S_OK)
								return FALSE;
							break;
						}
					}
				}
				else
				{
					hRes = m_decoder->SetOutputType(m_dwOutputID, outputType, 0);
					if (hRes != S_OK)
						return FALSE;
				}
			}
			else
			{
				if (outputType == NULL)
					return FALSE;
				hRes = m_decoder->SetOutputType(m_dwOutputID, outputType, 0);
				if (hRes != S_OK)
					return FALSE;
				TinyComPtr<IMFMediaType> mediaType;
				DWORD dwTypeIndex = 0;
				while (SUCCEEDED(m_decoder->GetInputAvailableType(m_dwInputID, dwTypeIndex++, &mediaType)))
				{
					BOOL bResult;
					hRes = mediaType->Compare(mediaType, MF_ATTRIBUTES_MATCH_OUR_ITEMS, &bResult);
					if (SUCCEEDED(hRes) && bResult)
					{
						hRes = m_decoder->SetInputType(m_dwInputID, mediaType, 0);
						if (hRes != S_OK)
							return FALSE;
						break;
					}
				}
			}
			hRes = m_decoder->GetInputStreamInfo(m_dwInputID, &m_inputInfo);
			if (hRes != S_OK)
				return FALSE;
			hRes = m_decoder->GetOutputStreamInfo(m_dwOutputID, &m_outputInfo);
			if (hRes != S_OK)
				return FALSE;
			return TRUE;
		}
		BOOL TinyMFDecode::Open(const GUID& clsID, IMFMediaType* inputType, IMFMediaType* outputType, Callback<void(BYTE*, LONG, LPVOID)>&& callback)
		{
			m_callback = std::move(callback);
			if (!Create(clsID, inputType, outputType))
				return FALSE;
			HRESULT hRes = S_OK;
			hRes = m_decoder->ProcessMessage(MFT_MESSAGE_COMMAND_FLUSH, NULL);
			if (hRes != S_OK)
				return FALSE;
			hRes = m_decoder->ProcessMessage(MFT_MESSAGE_NOTIFY_BEGIN_STREAMING, NULL);
			if (hRes != S_OK)
				return FALSE;
			hRes = m_decoder->ProcessMessage(MFT_MESSAGE_NOTIFY_START_OF_STREAM, NULL);
			if (hRes != S_OK)
				return FALSE;
			if (IsAsyncMFT(m_decoder, m_bIsAsync) && m_bIsAsync)
			{
				if (!this->Initialize(m_decoder))
					return FALSE;
			}
			return TRUE;
		}
		BOOL TinyMFDecode::CreateInputSample(const BYTE* bits, DWORD dwSize)
		{
			HRESULT hRes = S_OK;
			TinyComPtr<IMFMediaBuffer> buffer;
			if (!m_inputSample)
			{
				hRes = MFCreateSample(&m_inputSample);
				if (hRes != S_OK)
					return FALSE;
				if (m_inputInfo.cbAlignment > 0)
				{
					hRes = MFCreateAlignedMemoryBuffer(std::max<DWORD>(m_inputInfo.cbSize, dwSize), m_inputInfo.cbAlignment - 1, &buffer);
					if (hRes != S_OK)
						return FALSE;
				}
				else
				{
					hRes = MFCreateMemoryBuffer(std::max<DWORD>(m_inputInfo.cbSize, dwSize), &buffer);
					if (hRes != S_OK)
						return FALSE;
				}
				hRes = m_inputSample->AddBuffer(buffer);
				if (hRes != S_OK)
					return FALSE;
			}
			hRes = m_inputSample->GetBufferByIndex(0, &buffer);
			if (hRes != S_OK)
				return FALSE;
			DWORD dwMax = 0;
			hRes = buffer->GetMaxLength(&dwMax);
			if (hRes != S_OK)
				return FALSE;
			if (dwMax < dwSize)
			{
				hRes = m_inputSample->RemoveAllBuffers();
				if (hRes != S_OK)
					return FALSE;
				if (m_inputInfo.cbAlignment > 0)
				{
					hRes = MFCreateAlignedMemoryBuffer(std::max<DWORD>(m_inputInfo.cbSize, dwSize), m_inputInfo.cbAlignment - 1, &buffer);
					if (hRes != S_OK)
						return FALSE;
				}
				else
				{
					hRes = MFCreateMemoryBuffer(std::max<DWORD>(m_inputInfo.cbSize, dwSize), &buffer);
					if (hRes != S_OK)
						return FALSE;
				}
				hRes = m_inputSample->AddBuffer(buffer);
				if (hRes != S_OK)
					return FALSE;
			}
			BYTE* ps = NULL;
			hRes = buffer->Lock(&ps, NULL, NULL);
			if (hRes != S_OK)
				return FALSE;
			memcpy(ps, bits, dwSize);
			hRes = buffer->Unlock();
			if (hRes != S_OK)
				return FALSE;
			hRes = buffer->SetCurrentLength(dwSize);
			if (hRes != S_OK)
				return FALSE;
			return TRUE;
		}
		BOOL TinyMFDecode::CreateOutputSample(DWORD dwSize)
		{
			HRESULT hRes = S_OK;
			TinyComPtr<IMFMediaBuffer> buffer;
			if (!m_outputSample)
			{
				hRes = MFCreateSample(&m_outputSample);
				if (hRes != S_OK)
					return FALSE;
				if (m_outputInfo.cbAlignment > 0)
				{
					hRes = MFCreateAlignedMemoryBuffer(std::max<DWORD>(m_outputInfo.cbSize, dwSize), m_outputInfo.cbAlignment - 1, &buffer);
					if (hRes != S_OK)
						return FALSE;
				}
				else
				{
					hRes = MFCreateMemoryBuffer(std::max<DWORD>(m_outputInfo.cbSize, dwSize), &buffer);
					if (hRes != S_OK)
						return FALSE;
				}
				hRes = m_outputSample->AddBuffer(buffer);
				if (hRes != S_OK)
					return FALSE;
			}
			hRes = m_outputSample->GetBufferByIndex(0, &buffer);
			if (hRes != S_OK)
				return FALSE;
			DWORD dwMax = 0;
			hRes = buffer->GetMaxLength(&dwMax);
			if (hRes != S_OK)
				return FALSE;
			if (dwMax < dwSize)
			{
				hRes = m_outputSample->RemoveAllBuffers();
				if (hRes != S_OK)
					return FALSE;
				if (m_outputInfo.cbAlignment > 0)
				{
					hRes = MFCreateAlignedMemoryBuffer(std::max<DWORD>(m_outputInfo.cbSize, dwSize), m_outputInfo.cbAlignment - 1, &buffer);
					if (hRes != S_OK)
						return FALSE;
				}
				else
				{
					hRes = MFCreateMemoryBuffer(std::max<DWORD>(m_outputInfo.cbSize, dwSize), &buffer);
					if (hRes != S_OK)
						return FALSE;
				}
				hRes = m_outputSample->AddBuffer(buffer);
				if (hRes != S_OK)
					return FALSE;
			}
			return TRUE;
		}
		BOOL TinyMFDecode::GetOutputSample(DWORD dwSize)
		{
			ASSERT(m_decoder);
			for (;;)
			{
				if (m_bIsAsync)
				{
					if (m_outputs == 0)
						return TRUE;
					InterlockedDecrement(&m_outputs);
				}
				DWORD dwFlags = 0;
				HRESULT hRes = m_decoder->GetOutputStatus(&dwFlags);
				if (hRes != S_OK)
					return FALSE;
				if (dwFlags != MFT_OUTPUT_STATUS_SAMPLE_READY)
					break;
				MFT_OUTPUT_DATA_BUFFER samples = { 0 };
				if (!(m_outputInfo.dwFlags & (MFT_OUTPUT_STREAM_PROVIDES_SAMPLES | MFT_OUTPUT_STREAM_CAN_PROVIDE_SAMPLES)))
				{
					if (!CreateOutputSample(dwSize))
						return FALSE;
					samples.pSample = m_outputSample;
				}
				DWORD dwStatus;
				hRes = m_decoder->ProcessOutput(0, 1, &samples, &dwStatus);
				if (hRes != S_OK)
				{
					if (hRes != MF_E_TRANSFORM_NEED_MORE_INPUT)
						return FALSE;
					break;
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
				OnDataAvailable(pBuffer, dwCurrentSize, this);
				hRes = buffer->Unlock();
				if (hRes != S_OK)
					return FALSE;
			}
			return TRUE;
		}
		BOOL TinyMFDecode::Decode(const BYTE* bits, DWORD size)
		{
			if (!CreateInputSample(bits, size))
				return FALSE;
			if (m_bIsAsync)
			{
				if (m_inputs == 1 && m_sampleQueue.IsEmpty())
				{
					InterlockedDecrement(&m_inputs);
					HRESULT hRes = m_decoder->ProcessInput(m_dwInputID, m_inputSample, 0);
					if (hRes != S_OK)
						return FALSE;
					goto _LABEL1;
				}
				if (m_sampleQueue.GetSize() > 60)
				{
					m_sampleQueue.Clear();
					return FALSE;
				}
				IMFSample* sample = DuplicateSample(m_inputSample);
				if (sample != NULL)
				{
					m_sampleQueue.Push(sample);
					while (m_inputs > 0)
					{
						if (m_sampleQueue.IsEmpty())
						{
							break;
						}
						TinyComPtr<IMFSample> samplePop;
						if (m_sampleQueue.Pop(&samplePop))
						{
							InterlockedDecrement(&m_inputs);
							HRESULT hRes = m_decoder->ProcessInput(m_dwInputID, m_inputSample, 0);
							if (hRes != S_OK)
								return FALSE;
							goto _LABEL1;
						}
					}
				}
			}
			else
			{
				DWORD dwStatus = 0;
				HRESULT hRes = m_decoder->GetInputStatus(m_dwInputID, &dwStatus);
				if (MFT_INPUT_STATUS_ACCEPT_DATA != dwStatus)
					return TRUE;
				hRes = m_decoder->ProcessInput(m_dwInputID, m_inputSample, 0);
				if (hRes != S_OK)
					return FALSE;
			}
		_LABEL1:
			return GetOutputSample(size * 2);
		}
		BOOL TinyMFDecode::Close()
		{
			if (!m_decoder)
				return FALSE;
			HRESULT hRes = m_decoder->ProcessMessage(MFT_MESSAGE_NOTIFY_END_OF_STREAM, NULL);
			if (hRes != S_OK)
				return FALSE;
			hRes = m_decoder->ProcessMessage(MFT_MESSAGE_COMMAND_DRAIN, NULL);
			if (hRes != S_OK)
				return FALSE;
			hRes = m_decoder->ProcessMessage(MFT_MESSAGE_NOTIFY_END_STREAMING, NULL);
			if (hRes != S_OK)
				return FALSE;
			m_decoder.Release();
			return TRUE;
		}
		void TinyMFDecode::OnDataAvailable(BYTE* bits, LONG size, LPVOID lpParameter)
		{
			if (!m_callback.IsNull())
			{
				m_callback(bits, size, lpParameter);
			}
		}
	};
}

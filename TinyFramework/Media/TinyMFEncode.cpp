#include "../stdafx.h"
#include "TinyMFEncode.h"
#include <algorithm>
#include <limits>

namespace TinyFramework
{
	namespace Media
	{
		TinyMFEncode::TinyMFEncode()
			:m_bIsAsync(FALSE),
			m_dwInputID(0),
			m_dwOutputID(0)
		{

		}
		TinyMFEncode::~TinyMFEncode()
		{
			Close();
		}
		BOOL TinyMFEncode::IsEmpty() const
		{
			return m_transform == NULL;
		}
		BOOL TinyMFEncode::GetInputType(IMFMediaType** mediaType)
		{
			if (IsEmpty())
				return FALSE;
			return SUCCEEDED(m_transform->GetInputCurrentType(m_dwInputID, mediaType));
		}
		BOOL TinyMFEncode::GetOutputType(IMFMediaType** mediaType)
		{
			if (IsEmpty())
				return FALSE;
			return SUCCEEDED(m_transform->GetOutputCurrentType(m_dwOutputID, mediaType));
		}
		BOOL TinyMFEncode::SetMediaTypes(IMFMediaType* inputType, IMFMediaType* outputType)
		{
			if (IsEmpty())
				return FALSE;
			HRESULT hRes = S_OK;
			DWORD streams[2] = { 0 };
			hRes = m_transform->GetStreamCount(&streams[0], &streams[1]);
			if (hRes != S_OK)
				return FALSE;
			if (streams[0] == 0 || streams[1] == 0)
				return FALSE;
			TinyScopedArray<DWORD> dwInputIDs;
			TinyScopedArray<DWORD> dwOutputIDs;
			hRes = m_transform->GetStreamIDs(streams[0], dwInputIDs, streams[1], dwOutputIDs);
			if (hRes == S_OK)
			{
				m_dwInputID = dwInputIDs[0];
				m_dwOutputID = dwOutputIDs[0];
			}
			if (inputType != NULL)
			{
				hRes = m_transform->SetInputType(m_dwInputID, inputType, 0);
				if (hRes != S_OK)
					return FALSE;
				if (outputType == NULL)
				{
					TinyComPtr<IMFMediaType> mediaType;
					DWORD dwTypeIndex = 0;
					while (SUCCEEDED(m_transform->GetOutputAvailableType(m_dwInputID, dwTypeIndex++, &mediaType)))
					{
						BOOL bResult;
						hRes = mediaType->Compare(mediaType, MF_ATTRIBUTES_MATCH_OUR_ITEMS, &bResult);
						if (SUCCEEDED(hRes) && bResult)
						{
							hRes = m_transform->SetOutputType(m_dwOutputID, mediaType, 0);
							if (hRes != S_OK)
								return FALSE;
							break;
						}
					}
				}
				else
				{
					hRes = m_transform->SetOutputType(m_dwOutputID, outputType, 0);
					if (hRes != S_OK)
						return FALSE;
				}
			}
			else
			{
				if (outputType == NULL)
					return FALSE;
				hRes = m_transform->SetOutputType(m_dwOutputID, outputType, 0);
				if (hRes != S_OK)
					return FALSE;
				TinyComPtr<IMFMediaType> mediaType;
				DWORD dwTypeIndex = 0;
				while (SUCCEEDED(m_transform->GetInputAvailableType(m_dwInputID, dwTypeIndex++, &mediaType)))
				{
					BOOL bResult;
					hRes = mediaType->Compare(mediaType, MF_ATTRIBUTES_MATCH_OUR_ITEMS, &bResult);
					if (SUCCEEDED(hRes) && bResult)
					{
						hRes = m_transform->SetInputType(m_dwInputID, mediaType, 0);
						if (hRes != S_OK)
							return FALSE;
						break;
					}
				}
			}
			hRes = m_transform->ProcessMessage(MFT_MESSAGE_COMMAND_FLUSH, NULL);
			if (hRes != S_OK)
				return FALSE;
			hRes = m_transform->ProcessMessage(MFT_MESSAGE_NOTIFY_BEGIN_STREAMING, NULL);
			if (hRes != S_OK)
				return FALSE;
			hRes = m_transform->ProcessMessage(MFT_MESSAGE_NOTIFY_START_OF_STREAM, NULL);
			if (hRes != S_OK)
				return FALSE;
			return TRUE;
		}
		BOOL TinyMFEncode::CreateInputSample(const BYTE* bits, DWORD dwSize)
		{
			HRESULT hRes = S_OK;
			TinyComPtr<IMFMediaBuffer> buffer;
			if (!m_inputSample)
			{
				hRes = MFCreateSample(&m_inputSample);
				if (hRes != S_OK)
					return FALSE;
				MFT_INPUT_STREAM_INFO misi = { 0 };
				hRes = m_transform->GetInputStreamInfo(m_dwInputID, &misi);
				if (hRes != S_OK)
					return FALSE;
				if (misi.cbAlignment > 0)
				{
					hRes = MFCreateAlignedMemoryBuffer(std::max<DWORD>(misi.cbSize, dwSize), misi.cbAlignment - 1, &buffer);
					if (hRes != S_OK)
						return FALSE;
				}
				else
				{
					hRes = MFCreateMemoryBuffer(std::max<DWORD>(misi.cbSize, dwSize), &buffer);
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
				MFT_INPUT_STREAM_INFO misi = { 0 };
				hRes = m_transform->GetInputStreamInfo(m_dwInputID, &misi);
				if (hRes != S_OK)
					return FALSE;
				if (misi.cbAlignment > 0)
				{
					hRes = MFCreateAlignedMemoryBuffer(std::max<DWORD>(misi.cbSize, dwSize), misi.cbAlignment - 1, &buffer);
					if (hRes != S_OK)
						return FALSE;
				}
				else
				{
					hRes = MFCreateMemoryBuffer(std::max<DWORD>(misi.cbSize, dwSize), &buffer);
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
		BOOL TinyMFEncode::CreateOutputSample(DWORD dwSize)
		{
			m_growableIO.SetOffset(0);
			HRESULT hRes = S_OK;
			TinyComPtr<IMFMediaBuffer> buffer;
			if (!m_outputSample)
			{
				hRes = MFCreateSample(&m_outputSample);
				if (hRes != S_OK)
					return FALSE;
				MFT_OUTPUT_STREAM_INFO	mosi = { 0 };
				hRes = m_transform->GetOutputStreamInfo(m_dwOutputID, &mosi);
				if (hRes != S_OK)
					return FALSE;
				if (mosi.cbAlignment > 0)
				{
					hRes = MFCreateAlignedMemoryBuffer(std::max<DWORD>(mosi.cbSize, dwSize), mosi.cbAlignment - 1, &buffer);
					if (hRes != S_OK)
						return FALSE;
				}
				else
				{
					hRes = MFCreateMemoryBuffer(std::max<DWORD>(mosi.cbSize, dwSize), &buffer);
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
				MFT_OUTPUT_STREAM_INFO	mosi = { 0 };
				hRes = m_transform->GetOutputStreamInfo(m_dwOutputID, &mosi);
				if (hRes != S_OK)
					return FALSE;
				if (mosi.cbAlignment > 0)
				{
					hRes = MFCreateAlignedMemoryBuffer(std::max<DWORD>(mosi.cbSize, dwSize), mosi.cbAlignment - 1, &buffer);
					if (hRes != S_OK)
						return FALSE;
				}
				else
				{
					hRes = MFCreateMemoryBuffer(std::max<DWORD>(mosi.cbSize, dwSize), &buffer);
					if (hRes != S_OK)
						return FALSE;
				}
				hRes = m_outputSample->AddBuffer(buffer);
				if (hRes != S_OK)
					return FALSE;
			}
			return TRUE;
		}
		BOOL TinyMFEncode::GetOutputSample(DWORD dwSize, BYTE*& bo, DWORD& so)
		{
			if (IsEmpty())
				return FALSE;
			HRESULT hRes = S_OK;
			MFT_OUTPUT_STREAM_INFO	mosi = { 0 };
			hRes = m_transform->GetOutputStreamInfo(m_dwOutputID, &mosi);
			if (hRes != S_OK)
				return FALSE;
			for (;;)
			{
				DWORD dwFlags = 0;
				HRESULT hRes = m_transform->GetOutputStatus(&dwFlags);
				if (hRes != S_OK)
					return FALSE;
				if (dwFlags != MFT_OUTPUT_STATUS_SAMPLE_READY)
					break;
				MFT_OUTPUT_DATA_BUFFER samples = { 0 };
				if (!(mosi.dwFlags & (MFT_OUTPUT_STREAM_PROVIDES_SAMPLES | MFT_OUTPUT_STREAM_CAN_PROVIDE_SAMPLES)))
				{
					if (!CreateOutputSample(dwSize))
						return FALSE;
					samples.pSample = m_outputSample.Ptr();
				}
				DWORD dwStatus;
				hRes = m_transform->ProcessOutput(0, 1, &samples, &dwStatus);
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
				if (m_growableIO.RemainingCapacity() < static_cast<INT>(dwCurrentSize))
				{
					m_growableIO.SetCapacity(m_growableIO.capacity() + dwCurrentSize);
				}
				memcpy_s((void*)m_growableIO.data(), dwCurrentSize, pBuffer, dwCurrentSize);
				m_growableIO.SetOffset(m_growableIO.offset() + dwCurrentSize);
				hRes = buffer->Unlock();
				if (hRes != S_OK)
					return FALSE;
			}
			bo = (BYTE*)m_growableIO.StartOfBuffer();
			so = m_growableIO.offset();
			return TRUE;
		}
		BOOL TinyMFEncode::Open(const GUID& clsID)
		{
			HRESULT hRes = S_OK;
			TinyComPtr<IUnknown> unknow;
			hRes = unknow.CoCreateInstance(clsID, NULL, CLSCTX_INPROC_SERVER);
			if (hRes != S_OK)
				return FALSE;
			hRes = unknow->QueryInterface(IID_PPV_ARGS(&m_transform));
			if (hRes != S_OK)
				return FALSE;
			if (IsAsyncMFT(m_transform, m_bIsAsync) && m_bIsAsync)
			{
				hRes = m_transform->QueryInterface(&m_eventGenerator);
				if (hRes != S_OK)
					return FALSE;
			}
			return TRUE;
		}
		BOOL TinyMFEncode::Encode(SampleTag& tag, BYTE*& bo, DWORD& so)
		{
			if (!CreateInputSample(tag.bits, tag.size))
				return FALSE;
			TinyComPtr<IMFMediaType> mediaType;
			HRESULT hRes = m_transform->GetInputCurrentType(m_dwInputID, &mediaType);
			if (hRes != S_OK)
				return FALSE;
			hRes = m_inputSample->SetSampleTime(tag.timestamp);
			if (hRes != S_OK)
				return FALSE;
			hRes = m_inputSample->SetSampleDuration(tag.duration);
			if (hRes != S_OK)
				return FALSE;
			DWORD dwStatus = 0;
			hRes = m_transform->GetInputStatus(m_dwInputID, &dwStatus);
			if (MFT_INPUT_STATUS_ACCEPT_DATA != dwStatus)
				return TRUE;
			hRes = m_transform->ProcessInput(m_dwInputID, m_inputSample, 0);
			if (hRes != S_OK)
				return FALSE;
			return GetOutputSample(tag.size * 2, bo, so);
		}
		BOOL TinyMFEncode::Close()
		{
			if (!m_transform)
				return FALSE;
			HRESULT hRes = m_transform->ProcessMessage(MFT_MESSAGE_NOTIFY_END_OF_STREAM, NULL);
			if (hRes != S_OK)
				return FALSE;
			hRes = m_transform->ProcessMessage(MFT_MESSAGE_COMMAND_DRAIN, NULL);
			if (hRes != S_OK)
				return FALSE;
			hRes = m_transform->ProcessMessage(MFT_MESSAGE_NOTIFY_END_STREAMING, NULL);
			if (hRes != S_OK)
				return FALSE;
			m_transform.Release();
			return TRUE;
		}
	};
}

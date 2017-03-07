#include "../stdafx.h"
#include "TinyMFDecode.h"
#include <algorithm>
#include <limits>

namespace TinyUI
{
	namespace Media
	{
		TinyMFDecode::TinyMFDecode()
		{

		}
		TinyMFDecode::~TinyMFDecode()
		{
			Close();
		}
		BOOL TinyMFDecode::Create(const GUID& clsID, IMFMediaType* inputType, IMFMediaType* outputType)
		{
			HRESULT hRes = S_OK;
			TinyComPtr<IUnknown> unknow;
			hRes = unknow.CoCreateInstance(clsID, NULL, CLSCTX_INPROC_SERVER);
			if (FAILED(hRes))
				return FALSE;
			hRes = unknow->QueryInterface(IID_PPV_ARGS(&m_transform));
			if (FAILED(hRes))
				return FALSE;
			if (inputType != NULL)
			{
				hRes = m_transform->SetInputType(0, inputType, 0);
				if (FAILED(hRes))
					return FALSE;
				if (outputType == NULL)
				{
					TinyComPtr<IMFMediaType> mediaType;
					DWORD dwTypeIndex = 0;
					while (SUCCEEDED(m_transform->GetOutputAvailableType(0, dwTypeIndex++, &mediaType)))
					{
						BOOL bResult;
						hRes = mediaType->Compare(mediaType, MF_ATTRIBUTES_MATCH_OUR_ITEMS, &bResult);
						if (SUCCEEDED(hRes) && bResult)
						{
							hRes = m_transform->SetOutputType(0, mediaType, 0);
							if (FAILED(hRes))
								return FALSE;
							break;
						}
					}
				}
				else
				{
					hRes = m_transform->SetOutputType(0, outputType, 0);
					if (FAILED(hRes))
						return FALSE;
				}
			}
			else
			{
				if (outputType == NULL)
					return FALSE;
				hRes = m_transform->SetOutputType(0, outputType, 0);
				if (FAILED(hRes))
					return FALSE;
				TinyComPtr<IMFMediaType> mediaType;
				DWORD dwTypeIndex = 0;
				while (SUCCEEDED(m_transform->GetInputAvailableType(0, dwTypeIndex++, &mediaType)))
				{
					BOOL bResult;
					hRes = mediaType->Compare(mediaType, MF_ATTRIBUTES_MATCH_OUR_ITEMS, &bResult);
					if (SUCCEEDED(hRes) && bResult)
					{
						hRes = m_transform->SetInputType(0, mediaType, 0);
						if (FAILED(hRes))
							return FALSE;
						break;
					}
				}
			}
			hRes = m_transform->GetInputStreamInfo(0, &m_inputInfo);
			if (FAILED(hRes))
				return FALSE;
			hRes = m_transform->GetOutputStreamInfo(0, &m_outputInfo);
			if (FAILED(hRes))
				return FALSE;
			return TRUE;
		}
		BOOL TinyMFDecode::Open(const GUID& clsID, IMFMediaType* inputType, IMFMediaType* outputType, Callback<void(BYTE*, LONG, LPVOID)>&& callback)
		{
			m_callback = std::move(callback);
			if (!Create(clsID, inputType, outputType))
				return FALSE;
			HRESULT hRes = S_OK;
			hRes = m_transform->ProcessMessage(MFT_MESSAGE_COMMAND_FLUSH, NULL);
			if (FAILED(hRes))
				return FALSE;
			hRes = m_transform->ProcessMessage(MFT_MESSAGE_NOTIFY_BEGIN_STREAMING, NULL);
			if (FAILED(hRes))
				return FALSE;
			hRes = m_transform->ProcessMessage(MFT_MESSAGE_NOTIFY_START_OF_STREAM, NULL);
			if (FAILED(hRes))
				return FALSE;
			return TRUE;
		}
		BOOL TinyMFDecode::CreateInputSample(const BYTE* bits, DWORD dwSize)
		{
			HRESULT hRes = S_OK;
			TinyComPtr<IMFMediaBuffer> buffer;
			if (!m_inputSample)
			{
				hRes = MFCreateSample(&m_inputSample);
				if (FAILED(hRes))
					return FALSE;
				if (m_inputInfo.cbAlignment > 0)
				{
					hRes = MFCreateAlignedMemoryBuffer(std::max<DWORD>(m_inputInfo.cbSize, dwSize), m_inputInfo.cbAlignment - 1, &buffer);
					if (FAILED(hRes))
						return FALSE;
				}
				else
				{
					hRes = MFCreateMemoryBuffer(std::max<DWORD>(m_inputInfo.cbSize, dwSize), &buffer);
					if (FAILED(hRes))
						return FALSE;
				}
				hRes = m_inputSample->AddBuffer(buffer);
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
			if (dwMax < dwSize)
			{
				hRes = m_inputSample->RemoveAllBuffers();
				if (FAILED(hRes))
					return FALSE;
				if (m_inputInfo.cbAlignment > 0)
				{
					hRes = MFCreateAlignedMemoryBuffer(std::max<DWORD>(m_inputInfo.cbSize, dwSize), m_inputInfo.cbAlignment - 1, &buffer);
					if (FAILED(hRes))
						return FALSE;
				}
				else
				{
					hRes = MFCreateMemoryBuffer(std::max<DWORD>(m_inputInfo.cbSize, dwSize), &buffer);
					if (FAILED(hRes))
						return FALSE;
				}
				hRes = m_inputSample->AddBuffer(buffer);
				if (FAILED(hRes))
					return FALSE;
			}
			BYTE* ps = NULL;
			hRes = buffer->Lock(&ps, NULL, NULL);
			if (FAILED(hRes))
				return FALSE;
			memcpy(ps, bits, dwSize);
			hRes = buffer->Unlock();
			if (FAILED(hRes))
				return FALSE;
			hRes = buffer->SetCurrentLength(dwSize);
			if (FAILED(hRes))
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
				if (FAILED(hRes))
					return FALSE;
				if (m_outputInfo.cbAlignment > 0)
				{
					hRes = MFCreateAlignedMemoryBuffer(std::max<DWORD>(m_outputInfo.cbSize, dwSize), m_outputInfo.cbAlignment - 1, &buffer);
					if (FAILED(hRes))
						return FALSE;
				}
				else
				{
					hRes = MFCreateMemoryBuffer(std::max<DWORD>(m_outputInfo.cbSize, dwSize), &buffer);
					if (FAILED(hRes))
						return FALSE;
				}
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
				if (m_outputInfo.cbAlignment > 0)
				{
					hRes = MFCreateAlignedMemoryBuffer(std::max<DWORD>(m_outputInfo.cbSize, dwSize), m_outputInfo.cbAlignment - 1, &buffer);
					if (FAILED(hRes))
						return FALSE;
				}
				else
				{
					hRes = MFCreateMemoryBuffer(std::max<DWORD>(m_outputInfo.cbSize, dwSize), &buffer);
					if (FAILED(hRes))
						return FALSE;
				}
				hRes = m_outputSample->AddBuffer(buffer);
				if (FAILED(hRes))
					return FALSE;
			}
			return TRUE;
		}
		BOOL TinyMFDecode::GetOutputSample(DWORD dwSize)
		{
			ASSERT(m_transform);
			for (;;)
			{
				DWORD dwFlags = 0;
				HRESULT hRes = m_transform->GetOutputStatus(&dwFlags);
				if (FAILED(hRes))
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
			DWORD dwStatus = 0;
			HRESULT hRes = m_transform->GetInputStatus(0, &dwStatus);
			if (MFT_INPUT_STATUS_ACCEPT_DATA != dwStatus)
				return TRUE;
			hRes = m_transform->ProcessInput(0, m_inputSample, 0);
			if (FAILED(hRes))
				return FALSE;
			return GetOutputSample(size * 2);
		}
		BOOL TinyMFDecode::Close()
		{
			if (!m_transform)
				return FALSE;
			HRESULT hRes = m_transform->ProcessMessage(MFT_MESSAGE_NOTIFY_END_OF_STREAM, NULL);
			if (FAILED(hRes))
				return FALSE;
			hRes = m_transform->ProcessMessage(MFT_MESSAGE_COMMAND_DRAIN, NULL);
			if (FAILED(hRes))
				return FALSE;
			hRes = m_transform->ProcessMessage(MFT_MESSAGE_NOTIFY_END_STREAMING, NULL);
			if (FAILED(hRes))
				return FALSE;
			m_transform.Release();
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

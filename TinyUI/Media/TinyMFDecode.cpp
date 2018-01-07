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
			m_events[0].CreateEvent();
			m_events[1].CreateEvent();
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
			//Òì²½ÐèÒªUnlock
			if (IsAsyncMFT(m_decoder, m_bIsAsync) && m_bIsAsync)
			{
				UnlockAsyncMFT(m_decoder);
			}
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
			return TRUE;
		}
		BOOL TinyMFDecode::Open(const GUID& clsID, IMFMediaType* inputType, IMFMediaType* outputType)
		{
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
		BOOL TinyMFDecode::CreateInputSample(const BYTE* bits, DWORD cbSize)
		{
			MFT_INPUT_STREAM_INFO isi = { 0 };
			HRESULT hRes = m_decoder->GetInputStreamInfo(m_dwInputID, &isi);
			if (hRes != S_OK)
				return FALSE;
			TinyComPtr<IMFMediaBuffer> buffer;
			if (!m_inputSample)
			{
				hRes = MFCreateSample(&m_inputSample);
				if (hRes != S_OK)
					return FALSE;
				if (isi.cbAlignment > 0)
				{
					hRes = MFCreateAlignedMemoryBuffer(std::max<DWORD>(isi.cbSize, cbSize), isi.cbAlignment - 1, &buffer);
					if (hRes != S_OK)
						return FALSE;
				}
				else
				{
					hRes = MFCreateMemoryBuffer(std::max<DWORD>(isi.cbSize, cbSize), &buffer);
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
			if (dwMax < cbSize)
			{
				hRes = m_inputSample->RemoveAllBuffers();
				if (hRes != S_OK)
					return FALSE;
				if (isi.cbAlignment > 0)
				{
					hRes = MFCreateAlignedMemoryBuffer(std::max<DWORD>(isi.cbSize, cbSize), isi.cbAlignment - 1, &buffer);
					if (hRes != S_OK)
						return FALSE;
				}
				else
				{
					hRes = MFCreateMemoryBuffer(std::max<DWORD>(isi.cbSize, cbSize), &buffer);
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
			memcpy(ps, bits, cbSize);
			hRes = buffer->Unlock();
			if (hRes != S_OK)
				return FALSE;
			hRes = buffer->SetCurrentLength(cbSize);
			if (hRes != S_OK)
				return FALSE;
			return TRUE;
		}
		BOOL TinyMFDecode::CreateOutputSample(DWORD cbSize)
		{
			MFT_OUTPUT_STREAM_INFO osi = { 0 };
			HRESULT hRes = m_decoder->GetOutputStreamInfo(m_dwOutputID, &osi);
			if (hRes != S_OK)
				return FALSE;
			TinyComPtr<IMFMediaBuffer> buffer;
			if (!m_outputSample)
			{
				hRes = MFCreateSample(&m_outputSample);
				if (hRes != S_OK)
					return FALSE;
				if (osi.cbAlignment > 0)
				{
					hRes = MFCreateAlignedMemoryBuffer(std::max<DWORD>(osi.cbSize, cbSize), osi.cbAlignment - 1, &buffer);
					if (hRes != S_OK)
						return FALSE;
				}
				else
				{
					hRes = MFCreateMemoryBuffer(std::max<DWORD>(osi.cbSize, cbSize), &buffer);
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
			if (dwMax < cbSize)
			{
				hRes = m_outputSample->RemoveAllBuffers();
				if (hRes != S_OK)
					return FALSE;
				if (osi.cbAlignment > 0)
				{
					hRes = MFCreateAlignedMemoryBuffer(std::max<DWORD>(osi.cbSize, cbSize), osi.cbAlignment - 1, &buffer);
					if (hRes != S_OK)
						return FALSE;
				}
				else
				{
					hRes = MFCreateMemoryBuffer(std::max<DWORD>(osi.cbSize, cbSize), &buffer);
					if (hRes != S_OK)
						return FALSE;
				}
				hRes = m_outputSample->AddBuffer(buffer);
				if (hRes != S_OK)
					return FALSE;
			}
			return TRUE;
		}
		BOOL TinyMFDecode::GetOutputSample(DWORD estimate, BYTE*& bo, DWORD& so)
		{
			ASSERT(m_decoder);
			m_io.SetOffset(0);
			MFT_OUTPUT_STREAM_INFO osi = { 0 };
			HRESULT hRes = m_decoder->GetOutputStreamInfo(m_dwOutputID, &osi);
			if (hRes != S_OK)
				return FALSE;
			estimate = osi.cbSize > estimate ? osi.cbSize : estimate;
			for (;;)
			{
				DWORD dwFlags = 0;
				hRes = m_decoder->GetOutputStatus(&dwFlags);
				if (hRes == S_OK)
				{
					if (dwFlags != MFT_OUTPUT_STATUS_SAMPLE_READY)
						break;
				}
				MFT_OUTPUT_DATA_BUFFER samples = { 0 };
				if (!(osi.dwFlags & (MFT_OUTPUT_STREAM_PROVIDES_SAMPLES | MFT_OUTPUT_STREAM_CAN_PROVIDE_SAMPLES)))
				{
					if (!CreateOutputSample(estimate))
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
				hRes = buffer->GetCurrentLength(&so);
				if (hRes != S_OK)
					return FALSE;
				BYTE* pBuffer = NULL;
				hRes = buffer->Lock(&pBuffer, NULL, NULL);
				if (hRes != S_OK)
					return FALSE;
				if (m_io.RemainingCapacity() < so)
				{
					m_io.SetCapacity(m_io.capacity() + so);
				}
				memcpy_s((void*)m_io.data(), so, pBuffer, so);
				m_io.SetOffset(m_io.offset() + so);
				hRes = buffer->Unlock();
				if (hRes != S_OK)
					return FALSE;
			}
			bo = (BYTE*)m_io.StartOfBuffer();
			so = m_io.offset();
			return TRUE;
		}
		BOOL TinyMFDecode::CreateSample(const BYTE* bits, DWORD cbSize, IMFSample** sample)
		{
			MFT_INPUT_STREAM_INFO isi = { 0 };
			HRESULT hRes = m_decoder->GetInputStreamInfo(m_dwInputID, &isi);
			if (hRes != S_OK)
				return FALSE;
			TinyComPtr<IMFMediaBuffer> buffer;
			hRes = MFCreateSample(sample);
			if (hRes != S_OK)
				return FALSE;
			if (isi.cbAlignment > 0)
			{
				hRes = MFCreateAlignedMemoryBuffer(std::max<DWORD>(isi.cbSize, cbSize), isi.cbAlignment - 1, &buffer);
				if (hRes != S_OK)
					return FALSE;
			}
			else
			{
				hRes = MFCreateMemoryBuffer(std::max<DWORD>(isi.cbSize, cbSize), &buffer);
				if (hRes != S_OK)
					return FALSE;
			}
			hRes = (*sample)->AddBuffer(buffer);
			if (hRes != S_OK)
				return FALSE;
			hRes = (*sample)->GetBufferByIndex(0, &buffer);
			if (hRes != S_OK)
				return FALSE;
			DWORD dwMax = 0;
			hRes = buffer->GetMaxLength(&dwMax);
			if (hRes != S_OK)
				return FALSE;
			if (dwMax < cbSize)
			{
				hRes = (*sample)->RemoveAllBuffers();
				if (hRes != S_OK)
					return FALSE;
				if (isi.cbAlignment > 0)
				{
					hRes = MFCreateAlignedMemoryBuffer(std::max<DWORD>(isi.cbSize, cbSize), isi.cbAlignment - 1, &buffer);
					if (hRes != S_OK)
						return FALSE;
				}
				else
				{
					hRes = MFCreateMemoryBuffer(std::max<DWORD>(isi.cbSize, cbSize), &buffer);
					if (hRes != S_OK)
						return FALSE;
				}
				hRes = (*sample)->AddBuffer(buffer);
				if (hRes != S_OK)
					return FALSE;
			}
			BYTE* ps = NULL;
			hRes = buffer->Lock(&ps, NULL, NULL);
			if (hRes != S_OK)
				return FALSE;
			memcpy(ps, bits, cbSize);
			hRes = buffer->Unlock();
			if (hRes != S_OK)
				return FALSE;
			hRes = buffer->SetCurrentLength(cbSize);
			if (hRes != S_OK)
				return FALSE;
			return TRUE;
		}
		BOOL TinyMFDecode::CreateSample(DWORD cbSize, IMFSample** sample)
		{
			ASSERT(m_decoder);
			MFT_OUTPUT_STREAM_INFO osi = { 0 };
			HRESULT hRes = m_decoder->GetOutputStreamInfo(m_dwOutputID, &osi);
			if (hRes != S_OK)
				return FALSE;
			TinyComPtr<IMFMediaBuffer> buffer;
			hRes = MFCreateSample(sample);
			if (hRes != S_OK)
				return FALSE;
			if (osi.cbAlignment > 0)
			{
				hRes = MFCreateAlignedMemoryBuffer(std::max<DWORD>(osi.cbSize, cbSize), osi.cbAlignment - 1, &buffer);
				if (hRes != S_OK)
					return FALSE;
			}
			else
			{
				hRes = MFCreateMemoryBuffer(std::max<DWORD>(osi.cbSize, cbSize), &buffer);
				if (hRes != S_OK)
					return FALSE;
			}
			hRes = (*sample)->AddBuffer(buffer);
			if (hRes != S_OK)
				return FALSE;
			hRes = (*sample)->GetBufferByIndex(0, &buffer);
			if (hRes != S_OK)
				return FALSE;
			DWORD dwMax = 0;
			hRes = buffer->GetMaxLength(&dwMax);
			if (hRes != S_OK)
				return FALSE;
			if (dwMax < cbSize)
			{
				hRes = (*sample)->RemoveAllBuffers();
				if (hRes != S_OK)
					return FALSE;
				if (osi.cbAlignment > 0)
				{
					hRes = MFCreateAlignedMemoryBuffer(std::max<DWORD>(osi.cbSize, cbSize), osi.cbAlignment - 1, &buffer);
					if (hRes != S_OK)
						return FALSE;
				}
				else
				{
					hRes = MFCreateMemoryBuffer(std::max<DWORD>(osi.cbSize, cbSize), &buffer);
					if (hRes != S_OK)
						return FALSE;
				}
				hRes = (*sample)->AddBuffer(buffer);
				if (hRes != S_OK)
					return FALSE;
			}
			return TRUE;
		}
		BOOL TinyMFDecode::GetOutputSample(DWORD estimate, IMFSample** sample)
		{
			ASSERT(m_decoder);
			MFT_OUTPUT_STREAM_INFO osi = { 0 };
			HRESULT hRes = m_decoder->GetOutputStreamInfo(m_dwOutputID, &osi);
			if (hRes != S_OK)
				return FALSE;
			estimate = osi.cbSize > estimate ? osi.cbSize : estimate;
			DWORD dwFlags = 0;
			hRes = m_decoder->GetOutputStatus(&dwFlags);
			if (hRes == S_OK)
			{
				if (dwFlags != MFT_OUTPUT_STATUS_SAMPLE_READY)
					return FALSE;
			}
			MFT_OUTPUT_DATA_BUFFER samples = { 0 };
			if (!(osi.dwFlags & (MFT_OUTPUT_STREAM_PROVIDES_SAMPLES | MFT_OUTPUT_STREAM_CAN_PROVIDE_SAMPLES)))
			{
				if (!CreateSample(estimate, sample))
					return FALSE;
				samples.pSample = *sample;
			}
			DWORD dwStatus;
			hRes = m_decoder->ProcessOutput(0, 1, &samples, &dwStatus);
			if (hRes != S_OK)
			{
				if (hRes != MF_E_TRANSFORM_NEED_MORE_INPUT)
					return FALSE;
			}
			*sample = samples.pSample;
			return TRUE;
		}
		BOOL TinyMFDecode::Decode(SampleTag& tag, BYTE*& bo, DWORD& so)
		{
			DWORD dwStatus = 0;
			if (m_bIsAsync)
			{
				if (!m_events[0].Lock(INFINITE))
					return FALSE;
				if (!CreateInputSample(tag.bits, tag.size))
					return FALSE;
				HRESULT hRes = m_decoder->GetInputStatus(m_dwInputID, &dwStatus);
				if (MFT_INPUT_STATUS_ACCEPT_DATA != dwStatus)
					return TRUE;
				hRes = m_decoder->ProcessInput(m_dwInputID, m_inputSample, 0);
				if (hRes != S_OK)
					return FALSE;
				return GetOutputSample(tag.size * 2, bo, so);
			}
			else
			{
				if (!CreateInputSample(tag.bits, tag.size))
					return FALSE;
				HRESULT hRes = m_decoder->GetInputStatus(m_dwInputID, &dwStatus);
				if (MFT_INPUT_STATUS_ACCEPT_DATA != dwStatus)
					return TRUE;
				hRes = m_decoder->ProcessInput(m_dwInputID, m_inputSample, 0);
				if (hRes != S_OK)
					return FALSE;
				return GetOutputSample(tag.size * 2, bo, so);
			}
			return FALSE;
		}
		void TinyMFDecode::OnInvokeInput()
		{
			m_events[0].SetEvent();
			//	TinyAutoLock lock(m_lockI);
			//	if (m_inputs.GetSize() > 0)
			//	{
			//		ITERATOR s = m_inputs.First();
			//		SampleTag& sampleTag = m_inputs.GetAt(s);
			//		m_inputs.RemoveAt(s);
			//		if (!CreateInputSample(sampleTag.bits, sampleTag.size))
			//			goto _ERROR;
			//		DWORD dwStatus = 0;
			//		HRESULT hRes = m_decoder->GetInputStatus(m_dwInputID, &dwStatus);
			//		if (MFT_INPUT_STATUS_ACCEPT_DATA != dwStatus)
			//			goto _ERROR;
			//		hRes = m_decoder->ProcessInput(m_dwInputID, m_inputSample, 0);
			//		if (hRes != S_OK)
			//			goto _ERROR;
			//	}
			//_ERROR:
			//	INT a = 0;
			//	//TODO
		}
		void TinyMFDecode::OnInvokeOutput()
		{
			m_events[1].SetEvent();
			//	TinyAutoLock lock(m_lockO);
			//	DWORD so = 0;
			//	DWORD dwFlags = 0;
			//	DWORD dwStatus = 0;
			//	MFT_OUTPUT_STREAM_INFO osi = { 0 };
			//	MFT_OUTPUT_DATA_BUFFER samples = { 0 };
			//	TinyComPtr<IMFMediaBuffer> buffer;
			//	BYTE* pBuffer = NULL;
			//	HRESULT hRes = m_decoder->GetOutputStreamInfo(m_dwOutputID, &osi);
			//	if (hRes != S_OK)
			//		goto _ERROR;
			//	hRes = m_decoder->GetOutputStatus(&dwFlags);
			//	if (hRes == S_OK)
			//	{
			//		if (dwFlags != MFT_OUTPUT_STATUS_SAMPLE_READY)
			//			goto _ERROR;
			//	}
			//	if (!(osi.dwFlags & (MFT_OUTPUT_STREAM_PROVIDES_SAMPLES | MFT_OUTPUT_STREAM_CAN_PROVIDE_SAMPLES)))
			//	{
			//		if (!CreateOutputSample(0))
			//			goto _ERROR;
			//		samples.pSample = m_outputSample;
			//	}
			//	hRes = m_decoder->ProcessOutput(0, 1, &samples, &dwStatus);
			//	if (hRes != S_OK)
			//	{
			//		if (hRes != MF_E_TRANSFORM_NEED_MORE_INPUT)
			//			goto _ERROR;
			//	}	
			//	hRes = samples.pSample->ConvertToContiguousBuffer(&buffer);
			//	if (hRes != S_OK)
			//		goto _ERROR;
			//	hRes = buffer->GetCurrentLength(&so);
			//	if (hRes != S_OK)
			//		goto _ERROR;		
			//	hRes = buffer->Lock(&pBuffer, NULL, NULL);
			//	if (hRes != S_OK)
			//		goto _ERROR;
			//	hRes = buffer->Unlock();
			//	if (hRes != S_OK)
			//		goto _ERROR;
			//_ERROR:
			//	//TODO
			//	INT a = 0;
		}
		BOOL TinyMFDecode::Flush()
		{
			if (!m_decoder)
				return FALSE;
			HRESULT hRes = m_decoder->ProcessMessage(MFT_MESSAGE_COMMAND_FLUSH, NULL);
			if (hRes != S_OK)
				return FALSE;
			return TRUE;
		}
		BOOL TinyMFDecode::Drain()
		{
			if (!m_decoder)
				return FALSE;
			HRESULT hRes = m_decoder->ProcessMessage(MFT_MESSAGE_COMMAND_DRAIN, NULL);
			if (hRes != S_OK)
				return FALSE;
			return TRUE;
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
	};
}

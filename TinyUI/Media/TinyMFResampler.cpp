#include "../stdafx.h"
#include "TinyMFResampler.h"
#include <limits>
#include <algorithm>

namespace TinyUI
{
	namespace Media
	{
		TinyMFResampler::TinyMFResampler()
		{

		}
		TinyMFResampler::~TinyMFResampler()
		{
			Close();
		}
		BOOL TinyMFResampler::CreateResampler(const WAVEFORMATEX* pInputFormat, const WAVEFORMATEX* pOutputFormat)
		{
			HRESULT hRes = S_OK;
			TinyComPtr<IUnknown> unknow;
			hRes = unknow.CoCreateInstance(CLSID_CResamplerMediaObject, NULL, CLSCTX_INPROC_SERVER);
			if (hRes != S_OK)
				return FALSE;
			hRes = unknow->QueryInterface(IID_PPV_ARGS(&m_resampler));
			if (hRes != S_OK)
				return FALSE;
			TinyComPtr<IMFMediaType> inputMediaType;
			hRes = MFCreateMediaType(&inputMediaType);
			if (hRes != S_OK)
				return FALSE;
			hRes = MFInitMediaTypeFromWaveFormatEx(inputMediaType, pInputFormat, sizeof(WAVEFORMATEX) + pInputFormat->cbSize);
			if (hRes != S_OK)
				return FALSE;
			hRes = m_resampler->SetInputType(0, inputMediaType, 0);
			if (hRes != S_OK)
				return FALSE;
			TinyComPtr<IMFMediaType> outputMediaType;
			hRes = MFCreateMediaType(&outputMediaType);
			if (hRes != S_OK)
				return FALSE;
			hRes = MFInitMediaTypeFromWaveFormatEx(outputMediaType, pOutputFormat, sizeof(WAVEFORMATEX) + pOutputFormat->cbSize);
			if (hRes != S_OK)
				return FALSE;
			hRes = m_resampler->SetOutputType(0, outputMediaType, 0);
			if (hRes != S_OK)
				return FALSE;
			hRes = m_resampler->GetInputStreamInfo(0, &m_inputInfo);
			if (hRes != S_OK)
				return FALSE;
			hRes = m_resampler->GetOutputStreamInfo(0, &m_outputInfo);
			if (hRes != S_OK)
				return FALSE;
			TinyComPtr<IWMResamplerProps> resamplerProps;
			hRes = unknow->QueryInterface(IID_PPV_ARGS(&resamplerProps));
			if (hRes != S_OK)
				return FALSE;
			hRes = resamplerProps->SetHalfFilterLength(60);
			if (hRes != S_OK)
				return FALSE;
			return TRUE;
		}
		BOOL TinyMFResampler::Open(const WAVEFORMATEX* pFMTI, const WAVEFORMATEX* pFMTO, Callback<void(BYTE*, LONG, LPVOID)>&& callback)
		{
			ASSERT(pFMTI || pFMTO);
			m_callback = std::move(callback);
			m_waveFMTI = *pFMTI;
			m_waveFMTO = *pFMTO;
			if (!CreateResampler(pFMTI, pFMTO))
				return FALSE;
			HRESULT hRes = S_OK;
			hRes = m_resampler->ProcessMessage(MFT_MESSAGE_COMMAND_FLUSH, NULL);
			if (hRes != S_OK)
				return FALSE;
			hRes = m_resampler->ProcessMessage(MFT_MESSAGE_NOTIFY_BEGIN_STREAMING, NULL);
			if (hRes != S_OK)
				return FALSE;
			hRes = m_resampler->ProcessMessage(MFT_MESSAGE_NOTIFY_START_OF_STREAM, NULL);
			if (hRes != S_OK)
				return FALSE;
			return TRUE;
		}
		BOOL TinyMFResampler::CreateInputSample(const BYTE* bits, DWORD dwSize)
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
		BOOL TinyMFResampler::CreateOutputSample(DWORD dwSize)
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
		BOOL TinyMFResampler::GetOutputSample(DWORD dwSize)
		{
			ASSERT(m_resampler);
			for (;;)
			{
				MFT_OUTPUT_DATA_BUFFER samples = { 0 };
				if (!(m_outputInfo.dwFlags & (MFT_OUTPUT_STREAM_PROVIDES_SAMPLES | MFT_OUTPUT_STREAM_CAN_PROVIDE_SAMPLES)))
				{
					if (!CreateOutputSample(dwSize))
						return FALSE;
					samples.pSample = m_outputSample;
				}
				DWORD dwStatus;
				HRESULT hRes = m_resampler->ProcessOutput(0, 1, &samples, &dwStatus);
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
				OnDataAvailable(pBuffer, dwCurrentSize, this);
				hRes = buffer->Unlock();
				if (hRes != S_OK)
					return FALSE;
			}
			return TRUE;
		}
		BOOL TinyMFResampler::Resample(const BYTE* bits, DWORD size)
		{
			if (!m_resampler)
				return FALSE;
			if (!CreateInputSample(bits, size))
				return FALSE;
			DWORD dwStatus = 0;
			HRESULT hRes = m_resampler->GetInputStatus(0, &dwStatus);
			if (MFT_INPUT_STATUS_ACCEPT_DATA != dwStatus)
			{
				return TRUE;
			}
			hRes = m_resampler->ProcessInput(0, m_inputSample, 0);
			if (hRes != S_OK)
				return FALSE;
			DWORD dwSize = (DWORD)((LONGLONG)size * m_waveFMTO.nAvgBytesPerSec / m_waveFMTI.nAvgBytesPerSec);
			dwSize = (dwSize + (m_waveFMTO.nBlockAlign - 1)) / m_waveFMTO.nBlockAlign * m_waveFMTO.nBlockAlign;
			dwSize += 16 * m_waveFMTO.nBlockAlign;
			return GetOutputSample(dwSize);
		}
		BOOL TinyMFResampler::Close()
		{
			if (!m_resampler)
				return FALSE;
			HRESULT hRes = m_resampler->ProcessMessage(MFT_MESSAGE_NOTIFY_END_OF_STREAM, NULL);
			if (hRes != S_OK)
				return FALSE;
			hRes = m_resampler->ProcessMessage(MFT_MESSAGE_COMMAND_DRAIN, NULL);
			if (hRes != S_OK)
				return FALSE;
			hRes = m_resampler->ProcessMessage(MFT_MESSAGE_NOTIFY_END_STREAMING, NULL);
			if (hRes != S_OK)
				return FALSE;
			m_resampler.Release();
			return TRUE;
		}
		void TinyMFResampler::OnDataAvailable(BYTE* bits, LONG size, LPVOID lpParameter)
		{
			if (!m_callback.IsNull())
			{
				m_callback(bits, size, lpParameter);
			}
		}
	};
}

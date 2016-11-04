#include "../stdafx.h"
#include "TinyMFResampler.h"

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
			TinyComPtr<IUnknown> transform;
			hRes = transform.CoCreateInstance(CLSID_CResamplerMediaObject, NULL, CLSCTX_INPROC_SERVER);
			if (hRes != S_OK)
				return FALSE;
			hRes = transform->QueryInterface(IID_PPV_ARGS(&m_resampler));
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
			TinyComPtr<IWMResamplerProps> resamplerProps;
			hRes = transform->QueryInterface(IID_PPV_ARGS(&resamplerProps));
			if (hRes != S_OK)
				return FALSE;
			hRes = resamplerProps->SetHalfFilterLength(60);
			if (hRes != S_OK)
				return FALSE;
			return TRUE;
		}
		BOOL TinyMFResampler::Open(const WAVEFORMATEX* pInputFormat, const WAVEFORMATEX* pOutputFormat, Callback<void(BYTE*, LONG, LPVOID)>& callback)
		{
			ASSERT(pInputFormat || pOutputFormat);
			m_callback = std::move(callback);
			m_inputFormat = *pInputFormat;
			m_outputFormat = *pOutputFormat;
			if (!CreateResampler(pInputFormat, pOutputFormat))
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
		BOOL TinyMFResampler::CreateInputSample(const BYTE* bits, DWORD size, TinyComPtr<IMFSample>& sample)
		{
			HRESULT hRes = MFCreateSample(&sample);
			if (hRes != S_OK)
				return FALSE;
			TinyComPtr<IMFMediaBuffer> buffer;
			hRes = MFCreateMemoryBuffer(size, &buffer);
			if (hRes != S_OK)
				return FALSE;
			BYTE* ps = NULL;
			hRes = buffer->Lock(&ps, NULL, NULL);
			if (hRes != S_OK)
				return FALSE;
			memcpy(ps, bits, size);
			hRes = buffer->Unlock();
			if (hRes != S_OK)
				return FALSE;
			hRes = buffer->SetCurrentLength(size);
			if (hRes != S_OK)
				return FALSE;
			return sample->AddBuffer(buffer) == S_OK;
		}
		BOOL TinyMFResampler::CreateOutputSample(TinyComPtr<IMFSample>& sample, DWORD dwSize)
		{
			HRESULT hRes = MFCreateSample(&sample);
			if (hRes != S_OK)
				return FALSE;
			TinyComPtr<IMFMediaBuffer> buffer;
			hRes = MFCreateMemoryBuffer(dwSize, &buffer);
			if (hRes != S_OK)
				return FALSE;
			hRes = sample->AddBuffer(buffer);
			if (hRes != S_OK)
				return FALSE;
			return TRUE;
		}
		BOOL TinyMFResampler::GetOutputSample(TinyComPtr<IMFSample>& sample, DWORD dwSize)
		{
			ASSERT(m_resampler);
			TinyBufferArray sampleBuffer;
			for (;;)
			{
				MFT_OUTPUT_DATA_BUFFER samples = { 0 };
				TinyComPtr<IMFSample> s;
				if (!CreateOutputSample(s, dwSize))
					return FALSE;
				samples.pSample = s;
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
				sampleBuffer.Add(pBuffer, dwCurrentSize);
				hRes = buffer->Unlock();
				if (hRes != S_OK)
					return FALSE;
			}
			OnDataAvailable(sampleBuffer.m_value, sampleBuffer.m_size, this);
			return TRUE;
		}
		BOOL TinyMFResampler::Resample(const BYTE* bits, DWORD size)
		{
			TinyComPtr<IMFSample> inputSample;
			if (!CreateInputSample(bits, size, inputSample))
				return FALSE;
			DWORD dwStatus = 0;
			HRESULT hRes = m_resampler->GetInputStatus(0, &dwStatus);
			if (MFT_INPUT_STATUS_ACCEPT_DATA != dwStatus)
			{
				return TRUE;
			}
			hRes = m_resampler->ProcessInput(0, inputSample, 0);
			if (hRes != S_OK)
				return FALSE;
			DWORD dwSize = (DWORD)((LONGLONG)size * m_outputFormat.nAvgBytesPerSec / m_inputFormat.nAvgBytesPerSec);
			dwSize = (dwSize + (m_outputFormat.nBlockAlign - 1)) / m_outputFormat.nBlockAlign * m_outputFormat.nBlockAlign;
			dwSize += 16 * m_outputFormat.nBlockAlign;
			TinyComPtr<IMFSample> outputSample;
			return GetOutputSample(outputSample, dwSize);
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

#include "../stdafx.h"
#include "TinyResampler.h"

namespace TinyUI
{
	namespace Media
	{
		TinyResampler::TinyResampler()
		{

		}
		TinyResampler::~TinyResampler()
		{
			Close();
		}
		BOOL TinyResampler::CreateResampler(const WAVEFORMATEX* pInputType, const WAVEFORMATEX* pOutputType)
		{
			HRESULT hRes = S_OK;
			TinyComPtr<IUnknown> transform;
			hRes = transform.CoCreateInstance(CLSID_CResamplerMediaObject, NULL, CLSCTX_INPROC_SERVER);
			if (FAILED(hRes))
				return FALSE;
			hRes = transform->QueryInterface(IID_PPV_ARGS(&m_resampler));
			if (FAILED(hRes))
				return FALSE;
			TinyComPtr<IMFMediaType> inputMediaType;
			hRes = MFCreateMediaType(&inputMediaType);
			if (FAILED(hRes))
				return FALSE;
			hRes = MFInitMediaTypeFromWaveFormatEx(inputMediaType, pInputType, sizeof(WAVEFORMATEX) + pInputType->cbSize);
			if (FAILED(hRes))
				return FALSE;
			hRes = m_resampler->SetInputType(0, inputMediaType, 0);
			if (FAILED(hRes))
				return FALSE;
			TinyComPtr<IMFMediaType> outputMediaType;
			hRes = MFCreateMediaType(&outputMediaType);
			if (FAILED(hRes))
				return FALSE;
			hRes = MFInitMediaTypeFromWaveFormatEx(outputMediaType, pOutputType, sizeof(WAVEFORMATEX) + pOutputType->cbSize);
			if (FAILED(hRes))
				return FALSE;
			hRes = m_resampler->SetOutputType(0, outputMediaType, 0);
			if (FAILED(hRes))
				return FALSE;
			TinyComPtr<IWMResamplerProps> resamplerProps;
			hRes = transform->QueryInterface(IID_PPV_ARGS(&resamplerProps));
			if (FAILED(hRes))
				return FALSE;
			hRes = resamplerProps->SetHalfFilterLength(60);
			if (FAILED(hRes))
				return FALSE;
			return TRUE;
		}
		BOOL TinyResampler::Open(const WAVEFORMATEX* pInputFormat, const WAVEFORMATEX* pOutputFormat, Callback<void(BYTE*, LONG, LPVOID)>& callback)
		{
			ASSERT(pInputFormat || pOutputFormat);
			m_callback = std::move(callback);
			m_inputFormat = *pInputFormat;
			m_outputFormat = *pOutputFormat;
			if (!CreateResampler(pInputFormat, pOutputFormat))
				return FALSE;
			HRESULT hRes = S_OK;
			hRes = m_resampler->ProcessMessage(MFT_MESSAGE_COMMAND_FLUSH, NULL);
			if (FAILED(hRes))
				return FALSE;
			hRes = m_resampler->ProcessMessage(MFT_MESSAGE_NOTIFY_BEGIN_STREAMING, NULL);
			if (FAILED(hRes))
				return FALSE;
			hRes = m_resampler->ProcessMessage(MFT_MESSAGE_NOTIFY_START_OF_STREAM, NULL);
			if (FAILED(hRes))
				return FALSE;
			return TRUE;
		}
		BOOL TinyResampler::CreateInputSample(const BYTE* bits, DWORD size, TinyComPtr<IMFSample>& sample)
		{
			HRESULT hRes = MFCreateSample(&sample);
			if (FAILED(hRes))
				return FALSE;
			TinyComPtr<IMFMediaBuffer> buffer;
			hRes = MFCreateMemoryBuffer(size, &buffer);
			if (FAILED(hRes))
				return FALSE;
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
			return sample->AddBuffer(buffer) == S_OK;
		}
		BOOL TinyResampler::CreateOutputSample(TinyComPtr<IMFSample>& sample, DWORD dwSize)
		{
			HRESULT hRes = MFCreateSample(&sample);
			if (FAILED(hRes))
				return FALSE;
			TinyComPtr<IMFMediaBuffer> buffer;
			hRes = MFCreateMemoryBuffer(dwSize, &buffer);
			if (FAILED(hRes))
				return FALSE;
			hRes = sample->AddBuffer(buffer);
			if (FAILED(hRes))
				return FALSE;
			return TRUE;
		}
		BOOL TinyResampler::GetOutputSample(TinyComPtr<IMFSample>& sample, DWORD dwSize)
		{
			ASSERT(m_resampler);
			DWORD dwTotalSize = 0;
			DWORD dwOffsetSize = 0;
			BYTE* bits = NULL;
			for (;;)
			{
				MFT_OUTPUT_DATA_BUFFER samples = { 0 };
				TinyComPtr<IMFSample> s;
				if (!CreateOutputSample(s, dwSize))
					goto MTFERROR;
				samples.pSample = s;
				DWORD dwStatus;
				HRESULT hRes = m_resampler->ProcessOutput(0, 1, &samples, &dwStatus);
				if (FAILED(hRes))
				{
					if (hRes == MF_E_TRANSFORM_NEED_MORE_INPUT)
						break;
					else
						goto MTFERROR;
				}
				TinyComPtr<IMFMediaBuffer> buffer;
				hRes = samples.pSample->ConvertToContiguousBuffer(&buffer);
				if (FAILED(hRes))
					return FALSE;
				DWORD dwCurrentSize = 0;
				hRes = buffer->GetCurrentLength(&dwCurrentSize);
				if (FAILED(hRes))
					return FALSE;
				BYTE* pBuffer = NULL;
				hRes = buffer->Lock(&pBuffer, NULL, NULL);
				if (FAILED(hRes))
					goto MTFERROR;
				BYTE* myPtr = NULL;
				myPtr = (BYTE*)_recalloc(bits, dwTotalSize + dwCurrentSize, sizeof(BYTE*));
				bits = myPtr;
				memcpy(bits + dwOffsetSize, pBuffer, dwCurrentSize);
				dwOffsetSize = dwCurrentSize;
				dwTotalSize += dwCurrentSize;
				hRes = buffer->Unlock();
				if (FAILED(hRes))
					goto MTFERROR;
			}
			OnDataAvailable(bits, dwTotalSize, this);
		MTFERROR:
			SAFE_FREE(bits);
			return TRUE;
		}
		BOOL TinyResampler::Resample(const BYTE* bits, DWORD size)
		{
			TinyComPtr<IMFSample> inputSample;
			if (!CreateInputSample(bits, size, inputSample))
				return FALSE;
			DWORD dwStatus = 0;
			HRESULT hRes = m_resampler->GetInputStatus(0, &dwStatus);
			if (MFT_INPUT_STATUS_ACCEPT_DATA != dwStatus)
				return TRUE;
			hRes = m_resampler->ProcessInput(0, inputSample, 0);
			if (FAILED(hRes))
				return FALSE;
			DWORD dwOutputBytes = (DWORD)((LONGLONG)size * m_outputFormat.nAvgBytesPerSec / m_outputFormat.nAvgBytesPerSec);
			dwOutputBytes = (dwOutputBytes + (m_outputFormat.nBlockAlign - 1)) / m_outputFormat.nBlockAlign * m_outputFormat.nBlockAlign;
			dwOutputBytes += 16 * m_outputFormat.nBlockAlign;
			TinyComPtr<IMFSample> outputSample;
			return GetOutputSample(outputSample, dwOutputBytes);
		}
		BOOL TinyResampler::Close()
		{
			if (!m_resampler)
				return FALSE;
			HRESULT hRes = m_resampler->ProcessMessage(MFT_MESSAGE_NOTIFY_END_OF_STREAM, NULL);
			if (FAILED(hRes))
				return FALSE;
			hRes = m_resampler->ProcessMessage(MFT_MESSAGE_COMMAND_DRAIN, NULL);
			if (FAILED(hRes))
				return FALSE;
			hRes = m_resampler->ProcessMessage(MFT_MESSAGE_NOTIFY_END_STREAMING, NULL);
			if (FAILED(hRes))
				return FALSE;
			return TRUE;
		}
		void TinyResampler::OnDataAvailable(BYTE* bits, LONG size, LPVOID lpParameter)
		{
			if (!m_callback.IsNull())
			{
				m_callback(bits, size, lpParameter);
			}
		}
	};
}

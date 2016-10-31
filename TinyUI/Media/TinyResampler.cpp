#include "../stdafx.h"
#include "TinyResampler.h"

namespace TinyUI
{
	namespace Media
	{
		TinyResampler::TinyResampler()
		{
			MFStartup(MF_VERSION);

		}
		TinyResampler::~TinyResampler()
		{
			MFShutdown();
		}
		BOOL TinyResampler::Initialize(const WAVEFORMATEX* pTransformFrom, const WAVEFORMATEX* pTransformTo)
		{
			ASSERT(pTransformFrom || pTransformTo);
			TinyComPtr<IMFMediaType> mfFromType;
			HRESULT hRes = MFCreateMediaType(&mfFromType);
			if (FAILED(hRes))
				return FALSE;
			GUID val;
			hRes = mfFromType->GetGUID(MF_MT_SUBTYPE, &val);
			if (FAILED(hRes))
				return FALSE;
			hRes = TinyMFAPI::GetAudioDecoder(val, &m_transform);
			if (FAILED(hRes))
				return FALSE;
			hRes = m_transform->SetInputType(0, mfFromType, 0);
			if (FAILED(hRes))
				return FALSE;
			TinyComPtr<IMFMediaType> mfToType;
			hRes = MFCreateMediaType(&mfToType);
			if (FAILED(hRes))
				return FALSE;
			hRes = m_transform->SetOutputType(0, mfToType, 0);
			if (FAILED(hRes))
				return FALSE;
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
		BOOL TinyResampler::ProcessInput(BYTE* bits, DWORD size)
		{
			TinyComPtr<IMFSample> sample;
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
			sample->AddBuffer(buffer);
			hRes = m_transform->ProcessInput(0, sample, 0);
			if (FAILED(hRes))
				return FALSE;
			return TRUE;
		}
		BOOL TinyResampler::ProcessOutput(BYTE*& bits, DWORD& size)
		{
			HRESULT hRes = S_OK;
			for (;;)
			{
				MFT_OUTPUT_DATA_BUFFER output;
				DWORD dwStatus;
				hRes = m_transform->ProcessOutput(0, 1, &output, &dwStatus);
				if (hRes == MF_E_TRANSFORM_NEED_MORE_INPUT)
					break;
				IMFSample *pSample = output.pSample;
				TinyComPtr<IMFMediaBuffer> buffer;
				hRes = pSample->ConvertToContiguousBuffer(&buffer);
				if (FAILED(hRes))
					return FALSE;
				hRes = buffer->GetCurrentLength(&size);
				if (FAILED(hRes))
					return FALSE;
				BYTE* ps = NULL;
				hRes = buffer->Lock(&ps, NULL, NULL);
				if (FAILED(hRes))
					return FALSE;
				bits = new BYTE[size];
				memcpy(bits, ps, size);
				hRes = buffer->Unlock();
				if (FAILED(hRes))
					return FALSE;
			}
			return TRUE;
		}
	};
}

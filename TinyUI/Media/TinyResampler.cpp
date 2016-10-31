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
			HRESULT hRes = MFCreateMediaType(&mfToType);
			if (FAILED(hRes))
				return FALSE;
			hRes = m_transform->SetInputType(0, mfFromType, 0);
			if (FAILED(hRes))
				return FALSE;

			return TRUE;
		}
		BOOL TinyResampler::ProcessInput(BYTE* bits, LONG size)
		{
			TinyComPtr<IMFSample> sample;
			HRESULT hRes = MFCreateSample(sample);
			if (FAILED(hRes))
				return FALSE;
			TinyComPtr<IMFMediaBuffer> buffer;
			hRes = MFCreateMemoryBuffer(size, buffer);
			if (FAILED(hRes))
				return FALSE;
			hRes = buffer->Lock(bits, NULL, NULL);
			if (FAILED(hRes))
				return FALSE;
			hRes = buffer->Unlock();
			if (FAILED(hRes))
				return FALSE;
			m_transform->ProcessInput(0, sample);
		}
	};
}

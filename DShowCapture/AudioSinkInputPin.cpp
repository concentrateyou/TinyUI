#include "stdafx.h"
#include "AudioSinkInputPin.h"

namespace Media
{
	AudioSinkInputPin::AudioSinkInputPin(FilterBase* pFilter, FilterObserver* observer)
		:InputPinBase(pFilter, PIN_NAME, observer)
	{
	}
	AudioSinkInputPin::~AudioSinkInputPin()
	{
	}
	void AudioSinkInputPin::SetCaptureParam(const AudioCaptureParam& param)
	{
		m_param = param;
	}
	HRESULT AudioSinkInputPin::CheckMediaType(const AM_MEDIA_TYPE* pMediaType)
	{
		GUID type = pMediaType->majortype;
		if (type != MEDIATYPE_Audio)
			return E_INVALIDARG;
		GUID formatType = pMediaType->formattype;
		if (formatType != FORMAT_WaveFormatEx)
			return E_INVALIDARG;
		GUID subType = pMediaType->subtype;
		WAVEFORMATEX* pvi = reinterpret_cast<WAVEFORMATEX*>(pMediaType->pbFormat);
		if (pvi == NULL)
			return E_INVALIDARG;
		WAVEFORMATEX w = m_param.GetFormat();
		if (w.wFormatTag == pvi->wFormatTag &&
			w.wBitsPerSample == pvi->wBitsPerSample &&
			w.nSamplesPerSec == pvi->nSamplesPerSec &&
			w.nChannels == pvi->nChannels)
			return NOERROR;
		return S_FALSE;
	}
	HRESULT AudioSinkInputPin::GetMediaType(INT position, AM_MEDIA_TYPE* pMediaType)
	{
		if (position != 0)
			return E_INVALIDARG;
		if (pMediaType->cbFormat < sizeof(WAVEFORMATEX))
			return E_INVALIDARG;
		WAVEFORMATEX* pvi = reinterpret_cast<WAVEFORMATEX*>(pMediaType->pbFormat);
		*pvi = m_param.GetFormat();
		return NOERROR;
	}
}
#include "stdafx.h"
#include "AudioSinkInputPin.h"

namespace DShow
{
	AudioSinkInputPin::AudioSinkInputPin(FilterBase* pFilter, FilterObserver* observer)
		:InputPinBase(pFilter, PIN_NAME, observer)
	{
	}
	AudioSinkInputPin::~AudioSinkInputPin()
	{
	}
	const AudioCaptureFormat& AudioSinkInputPin::GetResponseFormat()
	{
		return m_response;
	}
	void AudioSinkInputPin::SetRequestFormat(const AudioCaptureFormat& request)
	{
		m_request = request;
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
		WAVEFORMATEX w = m_request.GetFormat();
		if (w.wFormatTag == pvi->wFormatTag &&
			w.wBitsPerSample == pvi->wBitsPerSample &&
			w.nSamplesPerSec == pvi->nSamplesPerSec &&
			w.nChannels == pvi->nChannels)
		{
			m_response = m_request;
			return NOERROR;
		}
		return E_FAIL;
	}
	HRESULT AudioSinkInputPin::GetMediaType(INT position, AM_MEDIA_TYPE* pMediaType)
	{
		if (position != 0)
			return E_INVALIDARG;
		if (pMediaType->cbFormat < sizeof(WAVEFORMATEX))
			return E_INVALIDARG;
		WAVEFORMATEX* pvi = reinterpret_cast<WAVEFORMATEX*>(pMediaType->pbFormat);
		*pvi = m_request.GetFormat();
		return NOERROR;
	}
}
#include "stdafx.h"
#include "LAVAudioInputPin.h"
#include "LAVAudio.h"

namespace LAV
{
	LAVAudioInputPin::LAVAudioInputPin(FilterBase* pFilter, FilterObserver* observer)
		:InputPinBase(pFilter, L"LAV Audio", observer)
	{
	}
	LAVAudioInputPin::~LAVAudioInputPin()
	{
	}
	const LAVAudioFormat& LAVAudioInputPin::GetResponseFormat()
	{
		return m_response;
	}
	void LAVAudioInputPin::SetRequestFormat(const LAVAudioFormat& request)
	{
		m_request = request;
	}
	HRESULT LAVAudioInputPin::CheckMediaType(const AM_MEDIA_TYPE* pMediaType)
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
	HRESULT LAVAudioInputPin::GetMediaType(INT position, AM_MEDIA_TYPE* pMediaType)
	{
		if (position != 0)
			return S_FALSE;
		if (pMediaType->cbFormat < sizeof(WAVEFORMATEX))
			return E_INVALIDARG;
		LAVAudio* ps = reinterpret_cast<LAVAudio*>(m_observer);
		if (ps != NULL)
		{
			TinyArray<ScopedMediaType> mediaTypes;
			if (!ps->GetOutputMediaTypes(mediaTypes))
				return E_FAIL;
			for (INT i = 0; i < mediaTypes.GetSize(); i++)
			{
				AM_MEDIA_TYPE* mediaType = mediaTypes[i].Ptr();
				if (mediaType->formattype == FORMAT_WaveFormatEx &&
					mediaType->subtype == MEDIASUBTYPE_PCM)
				{
					WAVEFORMATEX waveFMT = m_request.GetFormat();
					WAVEFORMATEX* s = reinterpret_cast<WAVEFORMATEX*>(mediaType->pbFormat);
					WAVEFORMATEX* pvi = reinterpret_cast<WAVEFORMATEX*>(pMediaType->pbFormat);
					if (s->nChannels == waveFMT.nChannels &&
						s->nSamplesPerSec == waveFMT.nSamplesPerSec&&
						s->wBitsPerSample == waveFMT.wBitsPerSample)
					{
						memcpy_s(pvi, sizeof(WAVEFORMATEX), s, sizeof(WAVEFORMATEX));
						return NOERROR;
					}
				}
			}
		}
		return E_FAIL;
	}
}
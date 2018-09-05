#include "stdafx.h"
#include "LAVAudio.h"

namespace LAV
{
	LAVAudio::LAVAudio(IGraphBuilder* builder, IPin* lavVideoO)
		:m_builder(builder),
		m_lavAudioO(lavVideoO)
	{
	}

	LAVAudio::~LAVAudio()
	{
	}

	BOOL LAVAudio::Initialize(const LAVAudioFormat& af)
	{
		if (!InitializeAudio())
			return FALSE;
		m_sinkFilter = new LAVAudioFilter(this);
		if (!m_sinkFilter)
			return FALSE;
		m_sinkFilter->SetRequestFormat(af);
		HRESULT hRes = m_builder->AddFilter(m_sinkFilter, NULL);
		if (hRes != S_OK)
			return FALSE;
		m_sinkI = m_sinkFilter->GetPin(0);
		if (!m_sinkI)
			return FALSE;
		m_audioI = GetPin(m_audioFilter, PINDIR_INPUT, GUID_NULL);
		if (!m_audioI)
			return FALSE;
		m_audioO = GetPin(m_audioFilter, PINDIR_OUTPUT, GUID_NULL);
		if (!m_audioO)
			return FALSE;
		hRes = m_builder->Connect(m_lavAudioO, m_audioI);
		if (hRes != S_OK)
			return FALSE;
		hRes = m_builder->Connect(m_audioO, m_sinkI);
		if (hRes != S_OK)
			return FALSE;
		return TRUE;
	}
	BOOL LAVAudio::Initialize(const LAVAudioFormat& af, Callback<void(BYTE*, LONG, REFERENCE_TIME, LPVOID)>&& callback)
	{
		if (!Initialize(af))
			return FALSE;
		m_callback = std::move(callback);
		return TRUE;
	}

	void LAVAudio::Uninitialize()
	{
		if (m_builder != NULL)
		{
			m_builder->Disconnect(m_audioI);
			m_builder->Disconnect(m_audioO);
			m_builder->Disconnect(m_sinkI);
			m_builder->RemoveFilter(m_sinkFilter);
			m_builder->RemoveFilter(m_audioFilter);
		}
		m_audioI.Release();
		m_audioO.Release();
		m_sinkI.Release();
		m_audioFilter.Release();
		m_sinkFilter = NULL;
	}
	BOOL LAVAudio::GetMediaType(AM_MEDIA_TYPE* pType)
	{
		if (!m_sinkI)
			return FALSE;
		return SUCCEEDED(m_sinkI->ConnectionMediaType(pType));
	}
	BOOL LAVAudio::GetAudioSettings(ILAVAudioSettings* settings)
	{
		if (!m_audioFilter)
			return FALSE;
		return SUCCEEDED(m_audioFilter->QueryInterface(__uuidof(ILAVAudioSettings), (void **)&settings));
	}
	const LAVAudioFormat& LAVAudio::GetResponseFormat() const
	{
		ASSERT(m_sinkFilter);
		return m_sinkFilter->GetResponseFormat();
	}
	BOOL LAVAudio::InitializeAudio()
	{
		if (!GetFilterByCLSID("{E8E73B6B-4CB3-44A4-BE99-4F7BCB96E491}", &m_audioFilter))
			return FALSE;
		HRESULT hRes = m_builder->AddFilter(m_audioFilter, NULL);
		if (hRes != S_OK)
			return FALSE;
		return TRUE;
	}
	BOOL LAVAudio::GetOutputMediaTypes(TinyArray<ScopedMediaType>& mediaTypes)
	{
		if (!m_audioO)
			return FALSE;
		if (!GetMediaTypes(m_audioO, mediaTypes))
			return FALSE;
		return TRUE;
	}
	BOOL LAVAudio::GetMediaTypes(IPin* pPin, TinyArray<ScopedMediaType>& mediaTypes)
	{
		TinyComPtr<IEnumMediaTypes> emts;
		if (FAILED(pPin->EnumMediaTypes(&emts)))
			return FALSE;
		emts->Reset();
		ULONG cFetched;
		AM_MEDIA_TYPE* pMediaType = NULL;
		while (emts->Next(1, &pMediaType, &cFetched) == NOERROR)
		{
			if (pMediaType->majortype == MEDIATYPE_Audio)
			{
				mediaTypes.Add(ScopedMediaType(pMediaType));
			}
		}
		return TRUE;
	}

	void LAVAudio::OnFrameReceive(BYTE* bits, LONG size, REFERENCE_TIME timestamp, LPVOID lpParameter)
	{
		if (!m_callback.IsNull())
		{
			m_callback(bits, size, timestamp, lpParameter);
		}
	}
}
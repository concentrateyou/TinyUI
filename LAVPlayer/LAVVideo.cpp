#include "stdafx.h"
#include "LAVVideo.h"

namespace LAV
{
	LAVVideo::LAVVideo(IGraphBuilder* builder, IPin* lavVideoO)
		:m_builder(builder),
		m_lavVideoO(lavVideoO)
	{
	}
	LAVVideo::~LAVVideo()
	{

	}
	BOOL LAVVideo::Initialize(const LAVVideoFormat& vf)
	{
		if (!InitializeVideo())
			return FALSE;
		m_sinkFilter = new LAVVideoFilter(this);
		if (!m_sinkFilter)
			return FALSE;
		m_sinkFilter->SetRequestFormat(vf);
		HRESULT hRes = m_builder->AddFilter(m_sinkFilter, NULL);
		if (hRes != S_OK)
			return FALSE;
		m_sinkI = m_sinkFilter->GetPin(0);
		if (!m_sinkI)
			return FALSE;
		m_videoI = GetPin(m_videoFilter, PINDIR_INPUT, GUID_NULL);
		if (!m_videoI)
			return FALSE;
		m_videoO = GetPin(m_videoFilter, PINDIR_OUTPUT, GUID_NULL);
		if (!m_videoO)
			return FALSE;
		hRes = m_builder->Connect(m_lavVideoO, m_videoI);
		if (hRes != S_OK)
			return FALSE;
		hRes = m_builder->Connect(m_videoO, m_sinkI);
		if (hRes != S_OK)
			return FALSE;
		return TRUE;
	}
	BOOL LAVVideo::Initialize(const LAVVideoFormat& vf, Callback<void(BYTE*, LONG, REFERENCE_TIME, LPVOID)>&& callback)
	{
		if (!Initialize(vf))
			return FALSE;
		m_callback = std::move(callback);
		return TRUE;
	}
	void LAVVideo::Uninitialize()
	{
		if (m_builder != NULL)
		{
			m_builder->Disconnect(m_videoO);
			m_builder->Disconnect(m_videoI);
			m_builder->Disconnect(m_sinkI);
			m_builder->RemoveFilter(m_videoFilter);
			m_builder->RemoveFilter(m_sinkFilter);
		}
		m_videoO.Release();
		m_videoI.Release();
		m_sinkI.Release();
		m_videoFilter.Release();
		m_sinkFilter = NULL;
	}
	BOOL LAVVideo::GetOutputMediaTypes(TinyArray<ScopedMediaType>& mediaTypes)
	{
		if (!GetMediaTypes(m_videoO, mediaTypes))
			return FALSE;
		return TRUE;
	}
	BOOL LAVVideo::GetMediaType(AM_MEDIA_TYPE* pType)
	{
		if (!m_sinkI)
			return FALSE;
		return SUCCEEDED(m_sinkI->ConnectionMediaType(pType));
	}
	BOOL LAVVideo::GetVideoSettings(ILAVVideoSettings* settings)
	{
		if (!m_videoFilter)
			return FALSE;
		return SUCCEEDED(m_videoFilter->QueryInterface(__uuidof(ILAVVideoSettings), (void **)&settings));
	}
	const LAVVideoFormat& LAVVideo::GetResponseFormat() const
	{
		ASSERT(m_sinkFilter);
		return m_sinkFilter->GetResponseFormat();
	}
	BOOL LAVVideo::InitializeVideo()
	{
		if (!GetFilterByCLSID("{EE30215D-164F-4A92-A4EB-9D4C13390F9F}", &m_videoFilter))
			return FALSE;
		HRESULT hRes = m_builder->AddFilter(m_videoFilter, NULL);
		if (hRes != S_OK)
			return FALSE;
		return TRUE;
	}
	BOOL LAVVideo::GetMediaTypes(IPin* pPin, TinyArray<ScopedMediaType>& mediaTypes)
	{
		TinyComPtr<IEnumMediaTypes> emts;
		if (FAILED(pPin->EnumMediaTypes(&emts)))
			return FALSE;
		emts->Reset();
		ULONG cFetched;
		AM_MEDIA_TYPE* pMediaType = NULL;
		while (emts->Next(1, &pMediaType, &cFetched) == NOERROR)
		{
			if (pMediaType->majortype == MEDIATYPE_Video)
			{
				mediaTypes.Add(ScopedMediaType(pMediaType));
			}
		}
		return TRUE;
	}
	void LAVVideo::OnFrameReceive(BYTE* bits, LONG size, REFERENCE_TIME timestamp, LPVOID lpParameter)
	{
		if (!m_callback.IsNull())
		{
			m_callback(bits, size, timestamp, lpParameter);
		}
	}
}
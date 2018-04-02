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
	BOOL LAVVideo::Initialize()
	{
		if (!InitializeVideo())
			return FALSE;
		m_sinkFilter = new LAVVideoFilter(this);
		if (!m_sinkFilter)
			return FALSE;
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
	BOOL LAVVideo::GetInputMediaType(AM_MEDIA_TYPE** ppType)
	{
		if (!GetMediaType(m_lavVideoO, ppType))
			return FALSE;
		return TRUE;
	}
	BOOL LAVVideo::GetOutputMediaType(AM_MEDIA_TYPE** ppType)
	{
		if (!GetMediaType(m_videoO, ppType))
			return FALSE;
		return TRUE;
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
	BOOL LAVVideo::GetMediaType(IPin* pPin, AM_MEDIA_TYPE** ppType)
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
				if (*ppType != NULL)
				{
					CopyMediaType(*ppType, pMediaType);
				}
				else
				{
					*ppType = pMediaType;
				}
				return TRUE;
			}
			DeleteMediaType(pMediaType);
		}
		return FALSE;
	}
	void LAVVideo::OnFrameReceive(BYTE* bits, LONG size, FLOAT ts, LPVOID lpParameter)
	{

	}

}
#include "stdafx.h"
#include "LAVVideo.h"

namespace LAV
{
	LAVVideo::LAVVideo(IGraphBuilder* builder, IPin* sourceVideoO)
		:m_builder(builder),
		m_sourceVideoO(sourceVideoO)
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
		m_sinkI = m_sinkFilter->GetPin(0);
		if (!m_sinkI)
			return FALSE;
		m_videoI = GetPin(m_videoFilter, PINDIR_INPUT, GUID_NULL);
		if (!m_videoI)
			return FALSE;
		m_videoO = GetPin(m_videoFilter, PINDIR_OUTPUT, GUID_NULL);
		if (!m_videoO)
			return FALSE;
		return TRUE;
	}
	void LAVVideo::Uninitialize()
	{

	}
	BOOL LAVVideo::InitializeVideo()
	{
		TinyComPtr<ICreateDevEnum> devEnum;
		HRESULT hRes = devEnum.CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC);
		if (hRes != S_OK)
			return FALSE;
		TinyComPtr<IEnumMoniker> enumMoniker;
		hRes = devEnum->CreateClassEnumerator(CLSID_LegacyAmFilterCategory, &enumMoniker, 0);
		if (hRes != S_OK)
			return FALSE;
		TinyComPtr<IMoniker> moniker;
		INT index = 0;
		while (enumMoniker->Next(1, &moniker, NULL) == S_OK)
		{
			TinyComPtr<IPropertyBag> propertyBag;
			hRes = moniker->BindToStorage(0, 0, IID_IPropertyBag, (void**)&propertyBag);
			if (hRes != S_OK)
			{
				moniker.Release();
				continue;
			}
			ScopedVariant variant;
			hRes = propertyBag->Read(L"CLSID", &variant, 0);
			if (hRes != S_OK)
			{
				moniker.Release();
				continue;
			}
			string clsid = std::move(WStringToString(V_BSTR(&variant)));
			if (clsid.compare("{EE30215D-164F-4A92-A4EB-9D4C13390F9F}") == 0)
			{
				hRes = moniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&m_videoFilter);
				if (hRes != S_OK || !m_videoFilter)
				{
					return FALSE;
				}
			}
		}
		return TRUE;
	}
	void LAVVideo::OnFrameReceive(BYTE* bits, LONG size, FLOAT ts, LPVOID lpParameter)
	{

	}

}
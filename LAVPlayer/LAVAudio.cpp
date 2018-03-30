#include "stdafx.h"
#include "LAVAudio.h"

namespace LAV
{
	LAVAudio::LAVAudio(IGraphBuilder* builder, IPin* sourceAudioO)
		:m_builder(builder),
		m_sourceAudioO(sourceAudioO)
	{
	}

	LAVAudio::~LAVAudio()
	{
	}

	BOOL LAVAudio::Initialize()
	{
		if (!InitializeAudio())
			return FALSE;
		m_sinkFilter = new LAVAudioFilter(this);
		if (!m_sinkFilter)
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

	BOOL LAVAudio::InitializeAudio()
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
			if (clsid.compare("{E8E73B6B-4CB3-44A4-BE99-4F7BCB96E491}") == 0)
			{
				hRes = moniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&m_audioFilter);
				if (hRes != S_OK || !m_audioFilter)
				{
					return FALSE;
				}
			}
		}
		return TRUE;
	}

	void LAV::LAVAudio::OnFrameReceive(BYTE* bits, LONG size, FLOAT ts, LPVOID lpParameter)
	{

	}

}
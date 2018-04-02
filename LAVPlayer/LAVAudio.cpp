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

	BOOL LAVAudio::Initialize()
	{
		if (!InitializeAudio())
			return FALSE;
		m_sinkFilter = new LAVAudioFilter(this);
		if (!m_sinkFilter)
			return FALSE;
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
		if (!GetFilterByCLSID("{E8E73B6B-4CB3-44A4-BE99-4F7BCB96E491}", &m_audioFilter))
			return FALSE;
		HRESULT hRes = m_builder->AddFilter(m_audioFilter, NULL);
		if (hRes != S_OK)
			return FALSE;
		return TRUE;
	}

	void LAV::LAVAudio::OnFrameReceive(BYTE* bits, LONG size, FLOAT ts, LPVOID lpParameter)
	{

	}

}
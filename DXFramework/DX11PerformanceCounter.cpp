#include "stdafx.h"
#include "DX11PerformanceCounter.h"

namespace DXFramework
{
	DX11PerformanceCounter::DX11PerformanceCounter(DX11& dx11)
		:m_dx11(dx11)
	{

	}

	DX11PerformanceCounter::~DX11PerformanceCounter()
	{

	}

	BOOL DX11PerformanceCounter::Initialize()
	{
		if (!m_dx11.GetD3D())
			return FALSE;
		ZeroMemory(&m_timestamp, sizeof(m_timestamp));
		m_timestamp.Frequency = 0;
		m_timestamp.Disjoint = FALSE;
		D3D11_QUERY_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
		desc.MiscFlags = 0;
		HRESULT hRes = m_dx11.GetD3D()->CreateQuery(&desc, &m_frequencyQuery);
		if (FAILED(hRes))
			return FALSE;
		desc.Query = D3D11_QUERY_TIMESTAMP;
		desc.MiscFlags = 0;
		hRes = m_dx11.GetD3D()->CreateQuery(&desc, &m_beginTimeQuery);
		if (FAILED(hRes))
			return FALSE;
		hRes = m_dx11.GetD3D()->CreateQuery(&desc, &m_endTimeQuery);
		if (FAILED(hRes))
			return FALSE;
		return TRUE;
	}
	BOOL DX11PerformanceCounter::BeginTime()
	{
		if (!m_dx11.GetImmediateContext())
			return FALSE;
		m_dx11.GetImmediateContext()->Begin(m_frequencyQuery);
		m_dx11.GetImmediateContext()->End(m_beginTimeQuery);
		return TRUE;
	}
	BOOL DX11PerformanceCounter::EndTime()
	{
		if (!m_dx11.GetImmediateContext())
			return FALSE;
		m_dx11.GetImmediateContext()->End(m_endTimeQuery);
		m_dx11.GetImmediateContext()->End(m_frequencyQuery);
		return TRUE;
	}
	BOOL DX11PerformanceCounter::GetTime(FLOAT& val)
	{
		if (!m_dx11.GetImmediateContext())
			return FALSE;
		while (m_dx11.GetImmediateContext()->GetData(m_frequencyQuery, NULL, 0, 0) == S_FALSE);
		m_dx11.GetImmediateContext()->GetData(m_frequencyQuery, &m_timestamp, sizeof(m_timestamp), 0);
		if (!(m_timestamp.Disjoint))
		{
			m_dx11.GetImmediateContext()->GetData(m_beginTimeQuery, &m_beginTime, sizeof(UINT64), 0);
			m_dx11.GetImmediateContext()->GetData(m_endTimeQuery, &m_endTime, sizeof(UINT64), 0);
			val = (FLOAT(m_endTime - m_beginTime) / FLOAT(m_timestamp.Frequency))*1000.0F;
			return TRUE;
		}
		return FALSE;
	}
}
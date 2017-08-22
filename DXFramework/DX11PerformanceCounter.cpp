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
		ZeroMemory(&m_timestamp, sizeof(m_timestamp));
		m_timestamp.Frequency = 0;
		m_timestamp.Disjoint = FALSE;
		D3D11_QUERY_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
		desc.MiscFlags = 0;
		HRESULT hr = m_dx11.GetD3D()->CreateQuery(&desc, &m_frequencyQuery);
		return TRUE;
	}
}
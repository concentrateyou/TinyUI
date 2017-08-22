#pragma once
#include "DXFramework.h"
#include "DX11.h"

namespace DXFramework
{
	class DX11PerformanceCounter
	{
		DISALLOW_COPY_AND_ASSIGN(DX11PerformanceCounter)
	public:
		DX11PerformanceCounter(DX11& dx11);
		~DX11PerformanceCounter();
		BOOL	Initialize();
		BOOL	BeginTime();
		BOOL	EndTime();
		BOOL	GetTime(FLOAT& val);
	private:
		DX11&					m_dx11;
		INT64					m_beginTime;
		INT64					m_endTime;
		TinyComPtr<ID3D11Query> m_frequencyQuery;
		TinyComPtr<ID3D11Query> m_beginTimeQuery;
		TinyComPtr<ID3D11Query> m_endTimeQuery;
		D3D11_QUERY_DATA_TIMESTAMP_DISJOINT m_timestamp;
	};
}




#include "stdafx.h"
#include "MonitorVisual2D.h"

namespace MediaSDK
{
	MonitorVisual2D::MonitorVisual2D(DX11& dx11)
		:m_dx11(dx11)
	{

	}


	MonitorVisual2D::~MonitorVisual2D()
	{
	}
	UINT MonitorVisual2D::GetOutputCount() const
	{
		return m_duplicator.GetOutputCount(m_dx11);
	}
	BOOL MonitorVisual2D::SetOutput(UINT index)
	{
		m_duplicator.Uninitialize();
		return m_duplicator.Initialize(m_dx11, index);
	}
	BOOL MonitorVisual2D::Open()
	{
		HANDLE handle = m_duplicator.GetSharedHandle();
		if (handle != NULL)
		{
			return m_visual2D.Load(m_dx11, handle);
		}
		return FALSE;
	}
	BOOL MonitorVisual2D::Process()
	{
		return m_duplicator.AcquireNextFrame(m_dx11, 0);
	}
	BOOL MonitorVisual2D::Close()
	{
		m_duplicator.Uninitialize();
		return TRUE;
	}
	LPCSTR	MonitorVisual2D::GetName()
	{
		return TEXT("MonitorVisual2D");
	}
	DX11Image2D* MonitorVisual2D::GetVisual2D()
	{
		return &m_visual2D;
	}
}
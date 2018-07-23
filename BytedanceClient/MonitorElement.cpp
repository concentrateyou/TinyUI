#include "stdafx.h"
#include "MonitorElement.h"

namespace Bytedance
{
	MonitorElement::MonitorElement(DX11& dx11)
		:m_dx11(dx11)
	{
	}
	MonitorElement::~MonitorElement()
	{

	}

	LPCSTR	MonitorElement::GetName()
	{
		return TEXT("MonitorElement");
	}

	UINT MonitorElement::GetMonitors() const
	{
		return m_duplicator.GetOutputs(m_dx11);
	}

	BOOL MonitorElement::Select(UINT index)
	{
		return	m_duplicator.Initialize(m_dx11, index);
	}

	BOOL MonitorElement::Open()
	{
		HANDLE handle = m_duplicator.GetSharedHandle();
		ASSERT(handle);
		return m_monitor2D.Load(m_dx11, handle);
	}

	BOOL MonitorElement::Process()
	{
		return m_duplicator.AcquireNextFrame(m_dx11, 0);
	}

	BOOL MonitorElement::Close()
	{
		m_duplicator.Uninitialize();
		return TRUE;
	}

	DX11Element* MonitorElement::GetVisual()
	{
		return static_cast<DX11Element*>(&m_monitor2D);
	}
}

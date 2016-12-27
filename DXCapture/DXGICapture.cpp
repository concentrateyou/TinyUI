#include "stdafx.h"
#include "DXGICapture.h"

namespace DXCapture
{
	DXGICapture::DXGICapture(DX10Capture& dx10, DX101Capture& dx101, DX11Capture& dx11)
		:m_bDX10(FALSE),
		m_bDX101(FALSE),
		m_bDX11(FALSE),
		m_dx10(dx10),
		m_dx101(dx101),
		m_dx11(dx11)
	{

	}
	DXGICapture::~DXGICapture()
	{

	}
	BOOL DXGICapture::Initialize(HWND hWND)
	{
		m_bDX10 = m_dx10.Initialize(hWND);
		if (m_bDX10)
			return TRUE;
		m_bDX101 = m_dx101.Initialize(hWND);
		if (m_bDX101)
			return TRUE;
		m_bDX11 = m_dx11.Initialize(hWND);
		if (m_bDX11)
			return TRUE;
		return FALSE;
	}
	void DXGICapture::Reset(BOOL bRelease)
	{
		if (m_bDX10)
			m_dx10.Reset(bRelease);
		if (m_bDX101)
			m_dx101.Reset(bRelease);
		if (m_bDX11)
			m_dx11.Reset(bRelease);
	}
}
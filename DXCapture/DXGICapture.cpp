#include "stdafx.h"
#include "DXGICapture.h"
#include "DX10Capture.h";
#include "DX101Capture.h";
#include "DX11Capture.h";

namespace DXCapture
{
	DXGICapture::DXGICapture()
		:m_bDX10(FALSE),
		m_bDX101(FALSE),
		m_bDX11(FALSE)
	{

	}
	DXGICapture::~DXGICapture()
	{

	}
	BOOL DXGICapture::Initialize(HWND hWND)
	{
		m_bDX10 = DX10Capture::Instance().Initialize(hWND);
		if (m_bDX10)
			return TRUE;
		m_bDX101 = DX101Capture::Instance().Initialize(hWND);
		if (m_bDX101)
			return TRUE;
		m_bDX11 = DX11Capture::Instance().Initialize(hWND);
		if (m_bDX11)
			return TRUE;
		return FALSE;
	}
	void DXGICapture::Reset(BOOL bRelease)
	{
		if (m_bDX10)
		{
			DX10Capture::Instance().Reset(bRelease);
		}
		if (m_bDX101)
		{
			DX101Capture::Instance().Reset(bRelease);
		}
		if (m_bDX11)
		{
			DX11Capture::Instance().Reset(bRelease);
		}
	}
	DXGICapture& DXGICapture::Instance()
	{
		static DXGICapture instance;
		return instance;
	}
}
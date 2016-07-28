#pragma once
#include "D3DCaptureSource.h"
#include "D3DDevice.h"

class CVideoCapture
{
public:
	CVideoCapture();
	~CVideoCapture();
	BOOL Initialize(HWND hWND, INT cx, INT cy);
	static DWORD MainCaptureLoop(LPVOID lpUnused);
private:
	D3D::CD3DDevice			m_device;
	D3D::CD3DCaptureSource  m_d3dCaptureSource;
	HANDLE					m_hCapture;
	DWORD					m_dwCurrentRenderTarget;
	TinyEvent				m_video;
};


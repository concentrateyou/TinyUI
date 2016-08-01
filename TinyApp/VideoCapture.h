#pragma once
#include "D3DCaptureSource.h"
#include "D3D10Device.h"

class CVideoCapture
{
public:
	CVideoCapture();
	~CVideoCapture();
	BOOL Initialize(HWND hWND, INT cx, INT cy);
	BOOL Save(LPCSTR pzFile);
	static DWORD MainCaptureLoop(LPVOID lpUnused);
private:
	D3D::CD3D10Device		m_device;
	D3D::CD3DCaptureSource  m_d3dCaptureSource;
	HANDLE					m_hCapture;
	DWORD					m_dwCurrentRenderTarget;
};


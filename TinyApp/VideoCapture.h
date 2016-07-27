#pragma once
#include "D3DCaptureSource.h"
#include "D3DSystem.h"

class CVideoCapture
{
public:
	CVideoCapture();
	~CVideoCapture();
	BOOL Initialize();
	static DWORD MainCaptureLoop(LPVOID lpUnused);
private:
	D3D::CD3DSystem			m_system;
	D3D::CD3DCaptureSource  m_d3dCaptureSource;
	HANDLE					m_hCapture;
	TinyEvent				m_video;
};


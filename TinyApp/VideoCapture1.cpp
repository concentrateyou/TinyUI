#include "stdafx.h"
#include "VideoCapture1.h"
using namespace D3D;

CVideoCapture::CVideoCapture()
	:m_d3dCaptureSource(m_device),
	m_hCapture(NULL),
	m_dwCurrentRenderTarget(0)
{

}

CVideoCapture::~CVideoCapture()
{

}

BOOL CVideoCapture::Initialize(HWND hWND, INT cx, INT cy)
{
	if (m_device.Initialize(hWND, cx, cy))
	{
		m_hCapture = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CVideoCapture::MainCaptureLoop, this, 0, 0);
		return m_hCapture != NULL;
	}
	return TRUE;
}

BOOL CVideoCapture::Save(LPCSTR pzFile)
{
	ASSERT(pzFile);
	return m_d3dCaptureSource.Save(pzFile);
}

DWORD CVideoCapture::MainCaptureLoop(LPVOID lpUnused)
{
	CVideoCapture* capture = reinterpret_cast<CVideoCapture*>(lpUnused);
	for (;;)
	{
		capture->m_d3dCaptureSource.Tick(0);
	}
	return FALSE;
}

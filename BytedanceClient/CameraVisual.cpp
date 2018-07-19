#include "stdafx.h"
#include "CameraVisual.h"

namespace Bytedance
{
	CameraVisual::CameraVisual(DX11& dx11)
		:m_dx11(dx11)
	{

	}
	CameraVisual::~CameraVisual()
	{

	}

	LPCSTR	CameraVisual::Name()
	{
		return TEXT("CameraVisual");
	}

	BOOL CameraVisual::Select(const VideoCapture::Name& name, const VideoCaptureParam& param)
	{
		m_capture.Uninitialize();
		if (!m_capture.Initialize(name))
			return FALSE;
		TinySize size = param.GetSize();
		if (!m_video.Create(m_dx11, size.cx, size.cy))
			return FALSE;
		m_param = param;
		return TRUE;
	}
	BOOL CameraVisual::Open()
	{
		if (!m_capture.Allocate(m_param))
			return FALSE;
		if (!m_capture.Start())
			return FALSE;
		return TRUE;
	}
	BOOL CameraVisual::Process()
	{
		if (m_video.IsEmpty())
			return FALSE;
		TinySize sizeT = m_param.GetSize();
		LONG size = m_capture.GetSize();
		BYTE* bits = m_capture.GetPointer();
		if (!bits)
			return FALSE;
		if (size <= 0)
			return FALSE;
		BYTE *pY = bits;
		BYTE *pU = pY + 1;
		BYTE *pV = pY + 3;
		if (!m_video.Copy(m_dx11, pY, sizeT.cx * 2, pU, sizeT.cx * 2, pV, sizeT.cx * 2))
			return FALSE;
		return FALSE;
	}
	BOOL CameraVisual::Close()
	{
		m_capture.Stop();
		m_capture.Deallocate();
		return TRUE;
	}

	DX11ImageElement2D* CameraVisual::visual()
	{
		return &m_video;
	}

}

#include "stdafx.h"
#include "CameraVisual.h"

namespace Bytedance
{
	IMPLEMENT_DYNAMIC(CameraVisual, DX10Image2D);

	CameraVisual::CameraVisual(DX10& dx10)
		:m_dx10(dx10)
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
		m_videoCapture.Uninitialize();
		if (!m_videoCapture.Initialize(name))
			return FALSE;
		if (!m_image2D.Create(m_dx10, m_param.GetSize().cx, m_param.GetSize().cy))
			return FALSE;
		m_param = param;
		return TRUE;
	}
	BOOL CameraVisual::Open()
	{
		if (!m_videoCapture.Allocate(m_param))
			return FALSE;
		if (!m_videoCapture.Start())
			return FALSE;
		return TRUE;
	}
	BOOL CameraVisual::Process()
	{
		BYTE* bits = m_videoCapture.GetPointer();
		LONG  size = m_videoCapture.GetSize();
		if (!m_image2D.Copy(m_dx10, bits, size, m_param.GetSize().cx * 4))
			return FALSE;
		return TRUE;
	}
	BOOL CameraVisual::Close()
	{
		m_videoCapture.Stop();
		m_videoCapture.Deallocate();
		return TRUE;
	}
}

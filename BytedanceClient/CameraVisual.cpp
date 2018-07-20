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

	BOOL CameraVisual::Select(const VideoCapture::Name& name, const VideoCaptureParam& requestParam)
	{
		m_capture.Uninitialize();
		m_capture.SetCallback(BindCallback(&CameraVisual::OnCallback, this));
		if (!m_capture.Initialize(name))
			return FALSE;
		m_requestParam = requestParam;
		return TRUE;
	}
	BOOL CameraVisual::Open()
	{
		this->Close();
		if (!m_capture.Allocate(m_requestParam))
			return FALSE;
		m_currentFormat = m_capture.GetCurrentFormat();
		if (!m_video.Create(m_dx11, m_currentFormat.GetSize().cx, m_currentFormat.GetSize().cy))
		{
			m_capture.DeAllocate();
			return FALSE;
		}
		if (!m_capture.Start())
			return FALSE;
		return TRUE;
	}
	BOOL CameraVisual::Process()
	{
		if (m_video.IsEmpty())
			return FALSE;
		return FALSE;
	}
	BOOL CameraVisual::Close()
	{
		m_capture.Stop();
		m_capture.DeAllocate();
		return TRUE;
	}

	DX11ImageElement2D* CameraVisual::visual()
	{
		return &m_video;
	}
	void CameraVisual::OnCallback(BYTE* bits, LONG size, FLOAT ts, void*)
	{

	}
}

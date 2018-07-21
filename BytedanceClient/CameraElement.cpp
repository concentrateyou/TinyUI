#include "stdafx.h"
#include "CameraElement.h"

namespace Bytedance
{
	CameraElement::CameraElement(DX11& dx11)
		:m_dx11(dx11)
	{
		m_linesize = 0;
	}
	CameraElement::~CameraElement()
	{

	}

	LPCSTR	CameraElement::name()
	{
		return TEXT("CameraVisual");
	}

	BOOL CameraElement::Select(const VideoCapture::Name& name, const VideoCaptureParam& requestParam)
	{
		m_capture.Uninitialize();
		m_capture.SetCallback(BindCallback(&CameraElement::OnCallback, this));
		if (!m_capture.Initialize(name))
			return FALSE;
		m_requestParam = requestParam;
		return TRUE;
	}
	BOOL CameraElement::Open()
	{
		this->Close();
		if (!m_capture.Allocate(m_requestParam))
			return FALSE;
		m_current = m_capture.GetFormat();
		TinySize size = m_current.GetSize();
		if (!m_videoYUY2.Create(m_dx11, size.cx, size.cy))
		{
			m_capture.Deallocate();
			return FALSE;
		}
		if (!m_videoRGB32.Create(m_dx11, size.cx, size.cy, NULL, FALSE))
		{
			m_capture.Deallocate();
			return FALSE;
		}
		m_videoRGB32.SetScale({ 0.5F,0.5F });
		m_videoRGB32.SetFlipV(TRUE);
		m_linesize = ((size.cx * 32 + 31) / 32) * 4;
		//m_linesize = ((size.cx + 1) / 2) * 4;//((size.cx * 32 + 31) / 32) * 4;
		if (!m_capture.Start())
			return FALSE;
		return TRUE;
	}
	BOOL CameraElement::Process()
	{
		/*if (m_videoYUY2.IsEmpty())
			return FALSE;
		if (!m_buffer.IsEmpty())
		{
			TinyAutoLock lock(m_lock);
			DWORD dwSize = m_ringBuffer.Read(m_buffer, 1);
			m_videoYUY2.Copy(m_dx11, m_buffer, m_linesize);
			return TRUE;
		}
		return FALSE;*/
		if (m_videoRGB32.IsEmpty())
			return FALSE;
		if (!m_buffer.IsEmpty())
		{
			TinyAutoLock lock(m_lock);
			DWORD dwSize = m_ringBuffer.Read(m_buffer, 1);
			if (dwSize > 0)
			{
				m_videoRGB32.Copy(m_dx11, m_buffer, m_linesize);
			}
			return TRUE;
		}
		return FALSE;
	}
	BOOL CameraElement::Close()
	{
		m_capture.Stop();
		m_capture.Deallocate();
		return TRUE;
	}

	DX11ImageElement2D* CameraElement::visual()
	{
		return &m_videoRGB32;
	}
	void CameraElement::OnCallback(BYTE* bits, LONG size, FLOAT ts, void*)
	{
		if (m_ringBuffer.IsEmpty())
		{
			m_buffer.Reset(size);
			m_ringBuffer.Initialize(3, size);
		}
		m_lock.Lock();
		m_ringBuffer.Write(bits, 1);
		m_lock.Unlock();
		//m_video.Copy(m_dx11, bits, m_linesize);
	}
}

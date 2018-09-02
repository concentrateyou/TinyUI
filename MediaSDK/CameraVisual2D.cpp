#include "stdafx.h"
#include "CameraVisual2D.h"

namespace MediaSDK
{
	IMPLEMENT_DYNAMIC(CameraVisual2D, IVisual2D);

	CameraVisual2D::CameraVisual2D(DX11& dx11)
		:m_dx11(dx11),
		m_linesize(0)
	{

	}
	CameraVisual2D::~CameraVisual2D()
	{
		Close();
	}

	void CameraVisual2D::SetTranslate(const XMFLOAT2& pos)
	{
		m_visual2D.SetTranslate(pos);
	}

	void CameraVisual2D::SetScale(const XMFLOAT2& pos)
	{
		m_visual2D.SetScale(pos);
	}

	XMFLOAT2 CameraVisual2D::GetTranslate()
	{
		return m_visual2D.GetTranslate();
	}

	XMFLOAT2 CameraVisual2D::GetSize()
	{
		return m_visual2D.GetSize();
	}

	XMFLOAT2 CameraVisual2D::GetScale()
	{
		return m_visual2D.GetScale();
	}

	LPCSTR	CameraVisual2D::GetVisualName()
	{
		return TEXT("CameraVisual2D");
	}

	BOOL CameraVisual2D::Select(const VideoCapture::Name& name, const VideoCaptureParam& requestParam)
	{
		m_capture.Uninitialize();
		m_capture.SetCallback(BindCallback(&CameraVisual2D::OnCallback, this));
		if (!m_capture.Initialize(name))
			return FALSE;
		m_requestParam = requestParam;
		return TRUE;
	}
	BOOL CameraVisual2D::Open()
	{
		this->Close();
		if (!m_capture.Allocate(m_requestParam))
			return FALSE;
		m_current = m_capture.GetFormat();
		TinySize size = m_current.GetSize();
		if (!m_visual2D.Create(m_dx11, size.cx, size.cy, NULL, FALSE))
		{
			m_capture.Deallocate();
			return FALSE;
		}
		m_visual2D.SetFlipV(TRUE);
		m_linesize = LINESIZE(32, size.cx);
		if (!m_capture.Start())
			return FALSE;
		return TRUE;
	}
	BOOL CameraVisual2D::Tick()
	{
		if (m_visual2D.IsEmpty())
			return FALSE;
		if (m_buffer.IsEmpty())
			return FALSE;
		FILTER_STATE state;
		m_capture.GetState(state);
		if (state != State_Running)
			return FALSE;
		TinyAutoLock autolock(m_lock);
		DWORD dwSize = m_ringBuffer.Read(m_buffer, 1);
		if (dwSize > 0)
		{
			m_visual2D.Copy(m_dx11, m_buffer, m_linesize);
		}
		return TRUE;
	}
	void CameraVisual2D::Close()
	{
		m_capture.Stop();
		m_capture.Deallocate();
		m_visual2D.Destory();
	}

	DX11Image2D* CameraVisual2D::GetVisual2D()
	{
		return &m_visual2D;
	}
	void CameraVisual2D::OnCallback(BYTE* bits, LONG size, REFERENCE_TIME timestamp, void*)
	{
		if (m_ringBuffer.IsEmpty())
		{
			m_buffer.Reset(size);
			m_ringBuffer.Initialize(3, size);
		}
		TinyAutoLock autolock(m_lock);
		m_ringBuffer.Write(bits, 1);
	}
}

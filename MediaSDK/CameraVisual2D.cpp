#include "stdafx.h"
#include "CameraVisual2D.h"

namespace MediaSDK
{
	IMPLEMENT_DYNAMIC(CameraVisual2D, IVisual2D);

	CameraVisual2D::CameraVisual2D(DX11& dx11)
		:m_dx11(dx11)
	{
		ZeroMemory(&m_sample, sizeof(m_sample));
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
		if (!m_visual2D.Create(m_dx11, size.cx, size.cy))
		{
			m_capture.Deallocate();
			return FALSE;
		}
		m_visual2D.SetFormat(PIXEL_FORMAT_YUY2);
		if (!m_capture.Start())
			return FALSE;
		return TRUE;
	}
	void CameraVisual2D::CopySample()
	{
		TinyAutoLock autolock(m_lock);
		if (m_samples.GetSize() > 0)
		{
			ITERATOR pos = m_samples.First();
			const VideoSample& sample = m_samples.GetAt(pos);
			memcpy_s(&m_sample, sizeof(VideoSample), &sample, sizeof(VideoSample));
			m_samples.RemoveAt(pos);
		}
	}
	BOOL CameraVisual2D::Tick(INT64& timestamp)
	{
		timestamp = -1;
		if (m_visual2D.IsEmpty())
			return FALSE;
		FILTER_STATE state;
		m_capture.GetState(state);
		if (state != State_Running)
			return FALSE;
		CopySample();
		if (m_sample.format == PIXEL_FORMAT_YUY2)
		{
			if (m_visual2D.Copy(m_dx11, m_sample.data[0], m_sample.linesize[0]))
			{
				timestamp = m_sample.timestamp;
			}
		}
		return TRUE;
	}
	void CameraVisual2D::Close()
	{
		m_capture.Stop();
		m_capture.Deallocate();
		m_visual2D.Destory();
	}

	DX11Element2D* CameraVisual2D::GetVisual2D()
	{
		return &m_visual2D;
	}
	void CameraVisual2D::OnCallback(BYTE* bits, LONG size, REFERENCE_TIME timestamp, void*)
	{
		VideoSample sample;
		ZeroMemory(&sample, sizeof(sample));
		TinySize sizeT = m_current.GetSize();
		sample.cx = static_cast<UINT32>(sizeT.cx);
		sample.cy = static_cast<UINT32>(sizeT.cy);
		sample.format = m_current.GetFormat();
		sample.timestamp = timestamp;
		switch (sample.format)
		{
		case PIXEL_FORMAT_RGB32:
			sample.data[0] = bits;
			sample.linesize[0] = sample.cx * 4;
			break;
		case PIXEL_FORMAT_YUY2:
		case PIXEL_FORMAT_UYVY:
			sample.data[0] = bits;
			sample.linesize[0] = sample.cx * 2;
			break;
		case PIXEL_FORMAT_I420:
			sample.data[0] = bits;
			sample.data[1] = sample.data[0] + (sample.cx * sample.cy);
			sample.data[2] = sample.data[1] + (sample.cx * sample.cy / 4);
			sample.linesize[0] = sample.cx;
			sample.linesize[1] = sample.cx / 2;
			sample.linesize[2] = sample.cx / 2;
			break;
		case PIXEL_FORMAT_YV12:
			sample.data[0] = bits;
			sample.data[2] = sample.data[0] + (sample.cx *  sample.cy);
			sample.data[1] = sample.data[2] + (sample.cx *  sample.cy / 4);
			sample.linesize[0] = sample.cx;
			sample.linesize[1] = sample.cx / 2;
			sample.linesize[2] = sample.cx / 2;
			break;
		case PIXEL_FORMAT_NV12:
			sample.data[0] = bits;
			sample.data[1] = sample.data[0] + (sample.cx *  sample.cy);
			sample.linesize[0] = sample.cx;
			sample.linesize[1] = sample.cx;
			break;
		default:
			return;
		}
		TinyAutoLock autolock(m_lock);
		m_samples.InsertLast(sample);
	}
}

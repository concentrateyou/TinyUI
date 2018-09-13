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
	void CameraVisual2D::SetName(LPCSTR pzName)
	{
		m_szname = pzName;
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

	LPCSTR	CameraVisual2D::GetName()
	{
		return m_szname.CSTR();
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
		TinySize sizeT = m_current.GetSize();
		if (!m_visual2D.Create(m_dx11, sizeT.cx, sizeT.cy))
		{
			m_capture.Deallocate();
			return FALSE;
		}
		if (!m_capture.Start())
		{
			m_capture.Deallocate();
			return FALSE;
		}
		return TRUE;
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
		BOOL bRes = FALSE;
		if (Pop(m_sample))
		{
			bRes = Copy(m_sample, timestamp);
			m_pool.Free(m_sample.data[0]);
		}
		else
		{
			bRes = Copy(m_sample, timestamp);
		}
		return bRes;
	}
	BOOL CameraVisual2D::Draw(DX11Graphics2D& g)
	{
		FLOAT blendFactor[4] = { 0.0F, 0.0F, 0.0F, 0.0F };
		m_dx11.AllowBlend(FALSE, blendFactor);
		return g.DrawImageYUY2BT601(m_visual2D);
	}
	void CameraVisual2D::Close()
	{
		m_capture.Stop();
		m_capture.Deallocate();
		m_visual2D.Destory();
	}
	BOOL CameraVisual2D::Copy(VideoSample& sample, INT64& timestamp)
	{
		switch (sample.iFormat)
		{
		case PIXEL_FORMAT_UNKNOWN:
			return FALSE;
		case PIXEL_FORMAT_YUY2:
		{
			if (m_visual2D.Copy(m_dx11, sample.data[0], sample.linesize[0]))
			{
				timestamp = sample.timestamp;
			}
		}
		break;
		}
		return TRUE;
	}
	BOOL CameraVisual2D::Pop(VideoSample& sample)
	{
		TinyAutoLock autolock(m_lock);
		if (m_samples.GetSize() <= 0)
			return FALSE;
		ITERATOR pos = m_samples.First();
		const VideoSample& sampleT = m_samples.GetAt(pos);
		memcpy_s(&sample, sizeof(VideoSample), &sampleT, sizeof(VideoSample));
		m_samples.RemoveAt(pos);
		return TRUE;
	}
	void CameraVisual2D::Push(BYTE* bits, LONG size, REFERENCE_TIME timestamp)
	{
		if (m_pool.IsEmpty())
		{
			m_pool.Initialize(3, size);
		}
		VideoSample sample;
		ZeroMemory(&sample, sizeof(sample));
		TinySize sizeT = m_current.GetSize();
		sample.cx = static_cast<UINT32>(sizeT.cx);
		sample.cy = static_cast<UINT32>(sizeT.cy);
		sample.iFormat = m_current.GetFormat();
		sample.size = size;
		sample.data[0] = static_cast<BYTE*>(m_pool.Alloc());
		memcpy_s(sample.data[0], size, bits, size);
		sample.timestamp = timestamp;
		switch (sample.iFormat)
		{
		case PIXEL_FORMAT_RGB32:
			sample.linesize[0] = sample.cx * 4;
			break;
		case PIXEL_FORMAT_YUY2:
		case PIXEL_FORMAT_UYVY:
			sample.linesize[0] = sample.cx * 2;
			break;
		case PIXEL_FORMAT_I420:
			sample.data[1] = sample.data[0] + (sample.cx * sample.cy);
			sample.data[2] = sample.data[1] + (sample.cx * sample.cy / 4);
			sample.linesize[0] = sample.cx;
			sample.linesize[1] = sample.cx / 2;
			sample.linesize[2] = sample.cx / 2;
			break;
		case PIXEL_FORMAT_YV12:
			sample.data[2] = sample.data[0] + (sample.cx *  sample.cy);
			sample.data[1] = sample.data[2] + (sample.cx *  sample.cy / 4);
			sample.linesize[0] = sample.cx;
			sample.linesize[1] = sample.cx / 2;
			sample.linesize[2] = sample.cx / 2;
			break;
		case PIXEL_FORMAT_NV12:
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
	void CameraVisual2D::OnCallback(BYTE* bits, LONG size, REFERENCE_TIME timestamp, void*)
	{
		Push(bits, size, timestamp);
	}
}

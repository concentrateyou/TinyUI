#include "stdafx.h"
#include "CameraVisual2D.h"

namespace MediaSDK
{
	IMPLEMENT_DYNAMIC(CameraVisual2D, IVisual2D);

	CameraVisual2D::CameraVisual2D(DX11& dx11)
		:m_dx11(dx11)
	{
		ZeroMemory(&m_package, sizeof(m_package));
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
		if (Pop(m_package))
		{
			bRes = Copy(m_package, timestamp);
			m_pool.Free(m_package.data[0]);
		}
		else
		{
			bRes = Copy(m_package, timestamp);
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
	BOOL CameraVisual2D::Copy(VideoPacket& package, INT64& timestamp)
	{
		switch (package.format)
		{
		case PIXEL_FORMAT_UNKNOWN:
			return FALSE;
		case PIXEL_FORMAT_YUY2:
		{
			if (m_visual2D.Copy(m_dx11, package.data[0], package.linesize[0]))
			{
				timestamp = package.timestamp;
			}
		}
		break;
		}
		return TRUE;
	}
	BOOL CameraVisual2D::Pop(VideoPacket& package)
	{
		TinyAutoLock autolock(m_lock);
		if (m_packages.GetSize() <= 0)
			return FALSE;
		ITERATOR pos = m_packages.First();
		const VideoPacket& sampleT = m_packages.GetAt(pos);
		memcpy_s(&package, sizeof(VideoPacket), &sampleT, sizeof(VideoPacket));
		m_packages.RemoveAt(pos);
		return TRUE;
	}
	void CameraVisual2D::Push(BYTE* bits, LONG size, REFERENCE_TIME timestamp)
	{
		if (m_pool.IsEmpty())
		{
			m_pool.Initialize(3, size);
		}
		VideoPacket package;
		ZeroMemory(&package, sizeof(package));
		TinySize sizeT = m_current.GetSize();
		package.cx = static_cast<UINT32>(sizeT.cx);
		package.cy = static_cast<UINT32>(sizeT.cy);
		package.format = m_current.GetFormat();
		package.size = size;
		package.data[0] = static_cast<BYTE*>(m_pool.Alloc());
		memcpy_s(package.data[0], size, bits, size);
		package.timestamp = timestamp;
		switch (package.format)
		{
		case PIXEL_FORMAT_RGB32:
			package.linesize[0] = package.cx * 4;
			break;
		case PIXEL_FORMAT_YUY2:
		case PIXEL_FORMAT_UYVY:
			package.linesize[0] = package.cx * 2;
			break;
		case PIXEL_FORMAT_I420:
			package.data[1] = package.data[0] + (package.cx * package.cy);
			package.data[2] = package.data[1] + (package.cx * package.cy / 4);
			package.linesize[0] = package.cx;
			package.linesize[1] = package.cx / 2;
			package.linesize[2] = package.cx / 2;
			break;
		case PIXEL_FORMAT_YV12:
			package.data[2] = package.data[0] + (package.cx *  package.cy);
			package.data[1] = package.data[2] + (package.cx *  package.cy / 4);
			package.linesize[0] = package.cx;
			package.linesize[1] = package.cx / 2;
			package.linesize[2] = package.cx / 2;
			break;
		case PIXEL_FORMAT_NV12:
			package.data[1] = package.data[0] + (package.cx *  package.cy);
			package.linesize[0] = package.cx;
			package.linesize[1] = package.cx;
			break;
		default:
			return;
		}
		TinyAutoLock autolock(m_lock);
		m_packages.InsertLast(package);
	}
	void CameraVisual2D::OnCallback(BYTE* bits, LONG size, REFERENCE_TIME timestamp, void*)
	{
		Push(bits, size, timestamp);
	}
}

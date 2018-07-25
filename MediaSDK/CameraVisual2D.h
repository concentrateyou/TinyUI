#pragma once
#include "MediaSDK.h"
#include "IVisual2D.h"

namespace MediaSDK
{
	class CameraVisual2D : public IVisual2D
	{
		DISALLOW_COPY_AND_ASSIGN(CameraVisual2D)
	public:
		CameraVisual2D(DX11& dx11);
		virtual ~CameraVisual2D();
	public:
		BOOL			Select(const VideoCapture::Name& name, const VideoCaptureParam& requestParam);
	public:
		BOOL			Open() OVERRIDE;
		BOOL			Process() OVERRIDE;
		void			Close() OVERRIDE;
		LPCSTR			GetName() OVERRIDE;
		DX11Image2D*	GetVisual2D() OVERRIDE;
	private:
		void			OnCallback(BYTE* bits, LONG size, FLOAT ts, void*);
	private:
		TinyLock			m_lock;
		TinyBuffer<BYTE>	m_buffer;
		UINT				m_linesize;
		DX11&				m_dx11;
		DX11Image2D			m_visual2D;
		TinyRingBuffer		m_ringBuffer;
		VideoCapture		m_capture;
		VideoCaptureParam	m_requestParam;
		VideoCaptureFormat	m_current;

	};
}




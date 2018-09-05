#pragma once
#include "MediaSDK.h"
#include "IVisual2D.h"
#include "VideoPacket.h"

namespace MediaSDK
{
	class CameraVisual2D : public IVisual2D
	{
		DECLARE_DYNAMIC(CameraVisual2D)
		DISALLOW_COPY_AND_ASSIGN(CameraVisual2D)
	public:
		CameraVisual2D(DX11& dx11);
		virtual ~CameraVisual2D();
	public:
		BOOL			Select(const VideoCapture::Name& name, const VideoCaptureParam& requestParam);
	public:
		BOOL			Open() OVERRIDE;
		BOOL			Tick(INT64& timestamp) OVERRIDE;
		void			Close() OVERRIDE;
		LPCSTR			GetVisualName() OVERRIDE;
		DX11Image2D*	GetVisual2D() OVERRIDE;
		XMFLOAT2		GetSize() OVERRIDE;

		XMFLOAT2		GetTranslate() OVERRIDE;
		XMFLOAT2		GetScale() OVERRIDE;
		void			SetTranslate(const XMFLOAT2& pos) OVERRIDE;
		void			SetScale(const XMFLOAT2& pos) OVERRIDE;
	private:
		void			CopySample();
		void			OnCallback(BYTE* bits, LONG size, REFERENCE_TIME timestamp, void*);
	private:
		DX11&						m_dx11;
		DX11Image2D					m_visual2D;
		VideoCapture				m_capture;
		VideoCaptureParam			m_requestParam;
		VideoCaptureFormat			m_current;
		VideoSample					m_sample;
		TinyLock					m_lock;
		TinyLinkList<VideoSample>	m_samples;
	};
}




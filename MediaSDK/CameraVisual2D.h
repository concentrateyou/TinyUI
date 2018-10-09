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
		BOOL			Draw(DX11Graphics2D& graphice2D) OVERRIDE;
		void			Close() OVERRIDE;
		LPCSTR			GetName() OVERRIDE;
		XMFLOAT2		GetSize() OVERRIDE;
		XMFLOAT2		GetTranslate() OVERRIDE;
		XMFLOAT2		GetScale() OVERRIDE;
		void			SetName(LPCSTR pzName) OVERRIDE;
		void			SetTranslate(const XMFLOAT2& pos) OVERRIDE;
		void			SetScale(const XMFLOAT2& pos) OVERRIDE;
	private:
		BOOL			Copy(VideoPacket& package, INT64& timestamp);
		BOOL			Pop(VideoPacket& package);
		void			Push(BYTE* bits, LONG size, REFERENCE_TIME timestamp);
		void			OnCallback(BYTE* bits, LONG size, REFERENCE_TIME timestamp, void*);
	private:
		DX11&						m_dx11;
		DX11YUY2Video				m_visual2D;
		TinyString					m_szname;
		TinyLock					m_lock;
		TinyMemoryPool				m_pool;
		TinyLinkList<VideoPacket>	m_packages;
		VideoCapture				m_capture;
		VideoCaptureParam			m_requestParam;
		VideoCaptureFormat			m_current;
		VideoPacket					m_package;
	};
}




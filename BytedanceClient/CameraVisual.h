#pragma once
#include "Common/TinyModule.h"
#include "IVisual.h"
#include "DX11.h"
#include "DX11CaptureRunner.h"
#include "DX11Image2D.h"
#include "DX11YUY2Video.h"
#include "DShowCommon.h"
#include "VideoCapture.h"
using namespace TinyFramework;
using namespace TinyFramework::IO;
using namespace DXFramework;
using namespace DShow;

namespace Bytedance
{
	/// <summary>
	/// 摄像头可视化
	/// </summary>
	class CameraVisual : public IVisual
	{
		DISALLOW_COPY_AND_ASSIGN(CameraVisual)
	public:
		CameraVisual(DX11& dx11);
		virtual ~CameraVisual();
		BOOL				Select(const VideoCapture::Name& name, const VideoCaptureParam& param);
		LPCSTR				Name() OVERRIDE;
		BOOL				Open() OVERRIDE;
		BOOL				Process() OVERRIDE;
		BOOL				Close() OVERRIDE;
		DX11ImageElement2D*	visual() OVERRIDE;
	private:
		void				OnCallback(BYTE* bits, LONG size, FLOAT ts, void*);
	private:
		TinyLock			m_lock;
		TinyBuffer<BYTE>	m_buffer;
		UINT				m_linesize;
		DX11&				m_dx11;
		DX11YUY2Video		m_video;
		VideoCapture		m_capture;
		VideoCaptureParam	m_requestParam;
		VideoCaptureFormat	m_current;
		TinyRingBuffer		m_ringBuffer;
	};
}



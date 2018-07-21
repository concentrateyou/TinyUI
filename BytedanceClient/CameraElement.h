#pragma once
#include "Common/TinyModule.h"
#include "IElement.h"
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
	class CameraElement : public IElement
	{
		DISALLOW_COPY_AND_ASSIGN(CameraElement)
	public:
		CameraElement(DX11& dx11);
		virtual ~CameraElement();
		BOOL				Select(const VideoCapture::Name& name, const VideoCaptureParam& param);
		LPCSTR				name() OVERRIDE;
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
		DX11YUY2Video		m_videoYUY2;
		DX11Image2D			m_videoRGB32;
		VideoCapture		m_capture;
		VideoCaptureParam	m_requestParam;
		VideoCaptureFormat	m_current;
		TinyRingBuffer		m_ringBuffer;
	};
}



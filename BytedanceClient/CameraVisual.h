#pragma once
#include "Common/TinyModule.h"
#include "IVisual.h"
#include "DX11.h"
#include "DX11CaptureRunner.h"
#include "DX11Image2D.h"
#include "DX11YUVVideo.h"
#include "DShowCommon.h"
#include "VideoCapture.h"
using namespace TinyFramework;
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
		DX11&				m_dx11;
		DX11YUVVideo		m_video;
		VideoCapture		m_capture;
		VideoCaptureParam	m_param;
	};
}



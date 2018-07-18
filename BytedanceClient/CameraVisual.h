#pragma once
#include "Common/TinyModule.h"
#include "IVisual.h"
#include "DX10.h"
#include "DX10CaptureRunner.h"
#include "DX10Image2D.h"
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
	class CameraVisual : public DX10Image2D, public IVisual
	{
		DECLARE_DYNAMIC(CameraVisual)
		DISALLOW_COPY_AND_ASSIGN(CameraVisual)
	public:
		CameraVisual(DX10& dx10);
		virtual ~CameraVisual();
		BOOL	Select(const VideoCapture::Name& name, const VideoCaptureParam& param);
		LPCSTR	Name() OVERRIDE;
		BOOL	Open() OVERRIDE;
		BOOL	Process() OVERRIDE;
		BOOL	Close() OVERRIDE;
	private:
		DX10&				m_dx10;
		DX10Image2D			m_image2D;
		VideoCapture		m_videoCapture;
		VideoCaptureParam	m_param;
	};
}



#pragma once
#include "VideoCapture.h"
#include "VideoCaptureParam.h"
#include "AudioCapture.h"
#include "AudioCaptureParam.h"
#include "DX11Image2D.h"
#include "DX11.h"

using namespace DXFramework;
using namespace DShow;

namespace DXApp
{
	/// <summary>
	/// …„œÒÕ∑≥°æ∞
	/// </summary>
	class VideoScene : public DX11Image2D
	{
		DECLARE_DYNAMIC(VideoScene)
	public:
		VideoScene();
		~VideoScene();
		BOOL Initialize(DX11& dx11, const VideoCapture::Name& name, const VideoCaptureParam& param);
	public:
		BOOL Allocate(DX11& dx11) OVERRIDE;
		BOOL Draw(DX11& dx11) OVERRIDE;
		void Deallocate(DX11& dx11) OVERRIDE;
	private:
		VideoCapture		m_video;
		VideoCaptureParam	m_param;
	};

}

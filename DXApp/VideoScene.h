#pragma once
#include "VideoCapture.h"
#include "VideoCaptureParam.h"
#include "AudioCapture.h"
#include "AudioCaptureParam.h"
#include "DX11Image.h"
#include "DX11.h"
using namespace DXFramework;
using namespace DShow;

namespace DXApp
{
	/// <summary>
	/// …„œÒÕ∑≥°æ∞
	/// </summary>
	class VideoScene : public DX11Image
	{
	public:
		VideoScene();
		~VideoScene();
		BOOL Initialize(DX11& dx11, const VideoCapture::Name& name, const VideoCaptureParam& param);
		BOOL BeginScene() OVERRIDE;
		void EndScene() OVERRIDE;
		BOOL Render(const DX11& dx11) OVERRIDE;
		LPCSTR GetClassName() OVERRIDE;
	private:
		VideoCapture		m_video;
		VideoCaptureParam	m_param;
	};

}

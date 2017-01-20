#include "stdafx.h"
#include "VideoScene.h"
#include "DX11Image.h"

namespace DXApp
{
	IMPLEMENT_DYNAMIC(VideoScene, DX11Image);
	VideoScene::VideoScene()
	{
	}


	VideoScene::~VideoScene()
	{
	}

	BOOL VideoScene::Initialize(DX11& dx11, const VideoCapture::Name& name, const VideoCaptureParam& param)
	{
		m_param = param;
		if (!m_video.Initialize(name))
			return FALSE;
		if (!m_video.Allocate(param))
			return FALSE;
		Destory();
		return DX11Image::Create(dx11, param.GetSize(), NULL);
	}

	BOOL VideoScene::Process(DX11& dx11)
	{
		return m_video.Start();
	}

	void VideoScene::Clear(DX11& dx11)
	{
		m_video.Uninitialize();
	}

	BOOL VideoScene::Render(DX11& dx11)
	{
		m_video.Lock();
		BYTE* bits = m_video.GetPointer();
		DX11Image::BitBlt(dx11, bits, m_video.GetSize());
		DX11Image::Render(dx11);
		m_video.Unlock();
		return bits != NULL;
	}
}
#include "stdafx.h"
#include "VideoScene.h"
#include "DX11Image2D.h"

namespace DXApp
{
	IMPLEMENT_DYNAMIC(VideoScene, DX11Image2D);

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
		return DX11Image2D::Create(dx11, param.GetSize(), TRUE);
	}

	BOOL VideoScene::Allocate(DX11& dx11)
	{
		return m_video.Start();
	}

	void VideoScene::Deallocate(DX11& dx11)
	{
		m_video.Uninitialize();
	}

	BOOL VideoScene::Draw(DX11& dx11)
	{
		m_video.Lock();
		BYTE* bits = m_video.GetPointer();
		LONG linesize = m_size.cx * 4;
		if (bits != NULL)
		{
			DX11Image2D::BitBlt(dx11, bits, m_video.GetSize(), linesize);
			DX11Image2D::Draw(dx11);
		}
		m_video.Unlock();
		return bits != NULL;
	}
}
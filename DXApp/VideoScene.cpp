#include "stdafx.h"
#include "VideoScene.h"
#include "DX11Image.h"

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
	return DX11Image::Create(dx11, param.GetSize(), NULL);
}

BOOL VideoScene::BeginScene()
{
	return m_video.Start();
}

void VideoScene::EndScene()
{
	m_video.Stop();
}

BOOL VideoScene::Render(const DX11& dx11)
{
	BYTE* bits = m_video.GetPointer();
	if (bits != NULL)
	{
		TinySize size = dx11.GetSize();
		this->BitBlt(dx11, bits);
		DX11Image::Render(dx11);
		return TRUE;
	}
	return FALSE;
}

LPCSTR VideoScene::GetClassName()
{
	return TEXT("VideoScene");
}

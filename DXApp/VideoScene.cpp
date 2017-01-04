#include "stdafx.h"
#include "VideoScene.h"


VideoScene::VideoScene()
{
}


VideoScene::~VideoScene()
{
}

BOOL VideoScene::Initialize(DX11& dx11, const VideoCapture::Name& name, const VideoCaptureParam& param)
{
	m_dx11 = dx11;
	if (!m_video.Initialize(name))
		return FALSE;
	if (!m_video.Allocate(param))
		return FALSE;
	if (!m_image.Create(dx11, param.GetSize().cx, param.GetSize().cy, param.GetScale().cx, param.GetScale().cy))
		return FALSE;
	return TRUE;
}
BOOL VideoScene::Scale(INT cx, INT cy)
{
	m_image.SetScale(cx, cy);
	return TRUE;
}
BOOL VideoScene::BeginScene()
{
	return m_video.Start();
}
BOOL VideoScene::EndScene()
{
	m_video.Uninitialize();
	return TRUE;
}
BOOL VideoScene::Render(DX11& dx11)
{
	BYTE* pData = m_video.GetPointer();
	if (pData != NULL)
	{
		TinySize size = dx11.GetSize();
		m_image.BitBlt(dx11, );
		m_graphics.DrawImage(m_image, size.cx * 2 / 3 + 1, 1);
	}
	
}
BOOL VideoScene::Scale(INT cx, INT cy)
{
	m_image.SetScale(cx, cy);
	return TRUE;
}
BOOL VideoScene::Move(INT x, INT y)
{
	m_image.SetPosition(x, y);
	return TRUE;
}

DX11Image* VideoScene::GetImage() const
{
	return m_image;
}

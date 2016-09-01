#pragma once
#include "DXFramework.h"
#include "DX11Image.h"
#include "DXCamera.h"
#include "DX11TextureShader.h"
#include "VideoCapture.h"
#include "DX11CaptureTask.h"
#include "RenderTask.h"
#include "PublishTask.h"
using namespace DXFramework;
using namespace Media;

class GraphicsCapture
{
public:
	GraphicsCapture();
	~GraphicsCapture();
	BOOL	Initialize(HWND hWND, INT cx, INT cy);
	BOOL	Resize(INT cx, INT cy);
	void	Render();
	void	Publish();
	BYTE*	GetPointer() const;
	DWORD	GetSize() const;
private:
	BOOL	CreatePublishTexture(INT cx, INT cy);
	BOOL	BuildEvents();
private:
	TinyComPtr<ID3D11Resource>		m_resource;//屏幕纹理
	DWORD							m_dwSize;//纹理大小
	TinyScopedPtr<BYTE>				m_bits;//纹理数据
	INT								m_cx;
	INT								m_cy;
	DX11							m_dx11;
	DX11Image						m_dxVideo;
	DX11Image						m_dxGame;
	DXCamera						m_camera;
	DX11TextureShader				m_textureShader;
	VideoCapture					m_videoCapture;
	TinyScopedPtr<PublishTask>		m_publishTask;
	TinyScopedPtr<RenderTask>		m_renderTask;
	TinyScopedPtr<DX11CaptureTask>	m_captureTask;
	TinyTaskPool					m_tasks;
};


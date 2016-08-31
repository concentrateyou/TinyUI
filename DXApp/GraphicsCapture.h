#pragma once
#include "DXFramework.h"
#include "DX11Image.h"
#include "DXCamera.h"
#include "DX11TextureShader.h"
#include "VideoCapture.h"
#include "DX11CaptureTask.h"
#include "RenderTask.h"
using namespace DXFramework;
using namespace Media;

class GraphicsCapture
{
public:
	GraphicsCapture();
	~GraphicsCapture();
	BOOL Initialize(HWND hWND, INT cx, INT cy);
	void Render();
private:
	DX11							m_dx11;
	DX11Image						m_dxVideo;
	DX11Image						m_dxGame;
	DXCamera						m_camera;
	DX11TextureShader				m_textureShader;
	VideoCapture					m_videoCapture;
	TinyScopedPtr<RenderTask>		m_renderTask;
	TinyScopedPtr<DX11CaptureTask>	m_captureTask;
	TinyTaskPool					m_tasks;
};


#pragma once
#include "DXFramework.h"
#include "DX11Image.h"
#include "DXCamera.h"
#include "DX11TextureShader.h"
#include "VideoCapture.h"
#include "AudioCapture.h"
#include "DX11CaptureTask.h"
#include "RenderTask.h"
#include "PublishTask.h"
#include "EncodeTask.h"
#include "RTMPPublisher.h"
using namespace DXFramework;
using namespace Media;

class DXGraphics : public TinyLock
{
public:
	DXGraphics();
	~DXGraphics();
	BOOL	Initialize(HWND hWND, INT cx, INT cy);
	BOOL	BeginScene();
	BOOL	EndScene();
	DX11&	GetD3D();
	BYTE*	GetPointer() const;
	BOOL	DrawImage(DX11Image& image, INT x, INT y);
private:
	BOOL	CreateTexture(INT cx, INT cy);
private:
	DX11							m_dx11;
	DX11TextureShader				m_textureShader;
	DXCamera						m_camera;
	TinyComPtr<ID3D11Resource>		m_resource;
	BYTE*							m_bits;
};


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
#include "I420Converter.h"
#include "aacEncode.h"
#include "x264Encode.h"
#include "RTMPPublisher.h"
using namespace DXFramework;
using namespace Media;

class GraphicsCapture : public TinyLock
{
public:
	GraphicsCapture();
	~GraphicsCapture();
	BOOL	Initialize(HWND hWND, INT cx, INT cy);
	void	Render();
	void	Publish();
	BYTE*	GetPointer() const;
	DWORD	GetSize() const;
	void	WaitAll();
private:
	BOOL	CreateTexture(INT cx, INT cy);
private:
	INT								m_cx;
	INT								m_cy;
	TinyComPtr<ID3D11Resource>		m_resource;
	BYTE*							m_bits;
	DX11							m_dx11;
	DX11Image						m_videoImage;
	DXCamera						m_camera;
	DX11TextureShader				m_textureShader;

	TinySize						m_videoSize;
	VideoCapture					m_videoCapture;
	AudioCapture					m_audioCapture;

	TinyScopedPtr<PublishTask>		m_publishTask;
	TinyScopedPtr<RenderTask>		m_renderTask;
	TinyScopedPtr<DX11CaptureTask>	m_captureTask;
	TinyScopedPtr<I420Converter>	m_converter;

	aacEncode						m_aacEncode;
	x264Encode						m_x264Encode;
	RTMPPublisher					m_publisher;
};


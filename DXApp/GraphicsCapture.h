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

class GraphicsCapture : public TinyLock
{
public:
	GraphicsCapture();
	~GraphicsCapture();
	BOOL	InitializeVideo(const VideoCapture::Name& name, const VideoCaptureParam& param);
	BOOL	InitializeAudio(const AudioCapture::Name& name, const AudioCaptureParam& param);
	BOOL	InitializeDX(HWND hWND, INT cx, INT cy);
	BOOL	Start();
	void	Render();
	BOOL	Stop();
	void	Publish();
	BYTE*	GetPointer() const;
	DWORD	GetSize() const;
	void	WaitAll();
private:
	BOOL	CreateTexture(INT cx, INT cy);
private:
	VideoCapture::Name				m_videoName;
	VideoCaptureParam				m_videoParam;
	AudioCapture::Name				m_audioName;
	AudioCaptureParam				m_audioParam;

	TinySize						m_size;
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

	TinyScopedPtr<EncodeTask>		m_encodeTask;

};


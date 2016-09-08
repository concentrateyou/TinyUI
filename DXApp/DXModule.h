#pragma once
#include "DXGraphics.h"

class DXModule
{
public:
	DXModule();
	~DXModule();
private:
	DXGraphics						m_graphics;
	MediaCapture					m_mediaCapture;
	TinyScopedPtr<PublishWorker>		m_publishTask;
	TinyScopedPtr<RenderWorker>		m_renderTask;
	TinyScopedPtr<DX11CaptureTask>	m_captureTask;
	TinyScopedPtr<EncodeWorker>		m_encodeTask;
};


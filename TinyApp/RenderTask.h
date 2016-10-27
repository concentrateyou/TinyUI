#pragma once
#include "IO/TinyTaskBase.h"
#include "IO/TinyRingQueue.h"
#include "DShowCommon.h"
#include "VideoCapture.h"
#include "Media/TinyWASAPI.h"
using namespace TinyUI;
using namespace TinyUI::IO;

class RenderTask : public TinyTaskBase
{
public:
	RenderTask(HWND hWND, DShow::VideoCapture* pVideo, const DShow::VideoCaptureParam& videpParam);
	~RenderTask();
	BOOL				Submit();
	BOOL				Close(DWORD dwMS) OVERRIDE;
private:
	void				OnMessagePump();
	void				OnExit();
private:
	
	DShow::VideoCapture*		m_pVideo;
	DShow::VideoCaptureParam	m_videoParam;
	HWND						m_hWND;
	TinyEvent					m_close;
};


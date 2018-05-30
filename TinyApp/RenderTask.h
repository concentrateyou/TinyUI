#pragma once
#include "IO/TinyThread.h"
#include "IO/TinyRingQueue.h"
#include "IO/TinyIO.h"
#include "DShowCommon.h"
#include "VideoCapture.h"
using namespace TinyFramework;
using namespace TinyFramework::IO;

class RenderTask : public TinyThread
{
public:
	RenderTask(HWND hWND, DShow::VideoCapture* pVideo, const DShow::VideoCaptureParam& videpParam);
	~RenderTask();
	BOOL				Submit();
	BOOL				Close(DWORD dwMS) OVERRIDE;
private:
	void				OnMessagePump();
	void				OnExit();
	void				Flush(BYTE* data, LONG size, INT);
private:
	IO::TinyFile				m_h264File;
	DShow::VideoCapture*		m_pVideo;
	DShow::VideoCaptureParam	m_videoParam;
	HWND						m_hWND;
	TinyEvent					m_close;
};


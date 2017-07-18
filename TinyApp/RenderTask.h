#pragma once
#include "IO/TinyTaskBase.h"
#include "IO/TinyRingQueue.h"
#include "IO/TinyIO.h"
#include "DShowCommon.h"
#include "VideoCapture.h"
#include "QSVConvert.h"
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
	void				Flush(BYTE* data, LONG size, INT);
private:
	IO::TinyFile				m_h264File;
	DShow::VideoCapture*		m_pVideo;
	DShow::VideoCaptureParam	m_videoParam;
	HWND						m_hWND;
	TinyEvent					m_close;
};


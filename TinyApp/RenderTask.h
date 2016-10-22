#pragma once
#include "IO/TinyTaskBase.h"
#include "IO/TinyRingQueue.h"
#include "DShowCommon.h"
#include "VideoCapture.h"
using namespace TinyUI;
using namespace TinyUI::IO;

class RenderTask : public TinyTaskBase
{
public:
	RenderTask(HWND hWND, Media::VideoCapture* pVideo, const Media::VideoCaptureParam& videpParam);
	// Media::AudioCapture* pAudio, const Media::AudioCaptureParam& audioParam
	//Media::AudioCapture*		m_pAudio;
	//Media::AudioCaptureParam	m_audioParam;
	~RenderTask();
	BOOL				Submit();
	BOOL				Close(DWORD dwMS) OVERRIDE;
private:
	void				OnMessagePump();
	void				OnExit();
private:
	Media::VideoCapture*		m_pVideo;
	Media::VideoCaptureParam	m_videoParam;
	HWND						m_hWND;
	TinyEvent					m_close;
};


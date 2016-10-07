#pragma once
#include "IO/TinyTaskBase.h"
#include "IO/TinyRingQueue.h"
#include "DShowCommon.h"
#include "VideoCapture.h"
using namespace TinyUI;
using namespace TinyUI::IO;

class CUIFrame;
class RenderTask : public TinyTaskBase
{
public:
	RenderTask(CUIFrame* pThis, HWND hWND, const Media::VideoCaptureParam& param);
	~RenderTask();
	BOOL				Submit();
	BOOL				Close(DWORD dwMS) OVERRIDE;
private:
	void				OnMessagePump();
	void				OnExit();
private:
	CUIFrame*					m_pThis;
	Media::VideoCaptureParam	m_param;
	HWND						m_hWND;
	TinyEvent					m_close;
};


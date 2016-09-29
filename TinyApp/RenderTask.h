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
	RenderTask(BYTE*& bits,HWND hWND,const Media::VideoCaptureParam& param);
	~RenderTask();
	BOOL				Submit();
	void				Exit() OVERRIDE;
private:
	void				OnMessagePump();
	void				OnExit();
private:
	BYTE*&						m_bits;
	Media::VideoCaptureParam	m_param;
	HWND						m_hWND;
};


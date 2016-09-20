#pragma once
#include "DXFramework.h"
#include "MediaCapture.h"
#include "DXGraphics.h"
#include "x264Encode.h"
#include "I420Converter.h"
#include "RTMPClient.h"
using namespace TinyUI::IO;
using namespace Media;

class VideoEncodeTask : public TinyTaskBase
{
public:
	VideoEncodeTask(DXGraphics* graphics, MediaCapture* capture);
	virtual ~VideoEncodeTask();
	BOOL		Open(INT cx, INT cy, INT scaleX, INT scaleY, DWORD dwFPS, DWORD dwVideoRate);
	BOOL		Submit();
	void		Exit() OVERRIDE;
	x264Encode*	GetEncode();
private:
	void		MessagePump();
private:
	INT												m_dwFPS;
	DWORD											m_dwTimestamp;
	vector<BYTE>									m_pps;
	vector<BYTE>									m_sps;
	x264Encode										m_x264;
	DXGraphics*										m_graphics;
	MediaCapture*									m_capture;
	TinyScopedPtr<I420Converter>					m_converter;
};


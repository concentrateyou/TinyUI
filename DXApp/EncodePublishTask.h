#pragma once
#include "DXFramework.h"
#include "Task.h"
#include "DXGraphics.h"
#include "MediaCapture.h"
#include "x264Encode.h"
#include "aacEncode.h"
#include "I420Converter.h"
#include "RTMPClient.h"
using namespace TinyUI::IO;

class EncodePublishTask : public TinyTaskBase
{
public:
	EncodePublishTask(DXGraphics* graphics, MediaCapture* capture);
	~EncodePublishTask();
	BOOL	Open(INT cx, INT cy, INT scaleX, INT scaleY, INT fps, INT rate, const WAVEFORMATEX& wfx);
	BOOL	Submit();
	void	Quit() OVERRIDE;
private:
	void	MessagePump();
	void	x264Done(BYTE*, INT, INT);
	void	aacDone(BYTE*, INT);
private:
	DXGraphics*		m_graphics;
	MediaCapture*	m_capture;
	TinyEvent		m_break;
	Closure			m_invoke;
	DWORD			m_dwTime;
private:
	x264Encode										m_x264;
	aacEncode										m_aac;
	TinyScopedPtr<I420Converter>					m_converter;
	TinyScopedPtr<Delegate<void(BYTE*, INT, INT)>>	m_x264Done;
	TinyScopedPtr<Delegate<void(BYTE*, INT)>>		m_aacDone;
	RTMPClient										m_client;
	vector<BYTE>									m_pps;
	vector<BYTE>									m_sps;
};


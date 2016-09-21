#pragma once
#include "DXFramework.h"
#include "I420Converter.h"
using namespace TinyUI;
extern "C"
{
#include "x264_config.h"
#include "x264.h"
}

#pragma comment(lib,"libx264.lib")
class x264Encode
{
public:
	x264Encode();
	~x264Encode();
	BOOL	Open(INT cx, INT cy, INT fps = 50, INT rate = 1000);//rateÆ½¾ùÂëÂÊ
	BOOL	Encode(AVFrame* pI420);
	void	Close();
	LONG	GetTimespan() const;
public:
	virtual void OnDone(BYTE*, INT, INT);
public:
	Event<void(BYTE*, INT, INT)> EVENT_DONE;
private:
	BOOL	BuildParam(INT cx, INT cy, INT fps, INT bitrate);
private:
	x264_t*				m_x264;
	x264_param_t*		m_x264Param;
	x264_picture_t*		m_x264Image;
};


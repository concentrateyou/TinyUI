#pragma once
#include "DXFramework.h"
#include "Common/TinyTime.h"
#include "I420Converter.h"
#include "aacEncode.h"
#include "x264Encode.h"
using namespace TinyUI::IO;

class GraphicsCapture;
class EncodeTask : public TinyTask
{
public:
	EncodeTask(GraphicsCapture* pSys);
	~EncodeTask();
	BOOL	Submit();
private:
	void	MessagePump();
private:
	TinyScopedPtr<I420Converter>	m_converter;
	GraphicsCapture*				m_pSys;
	aacEncode						m_aacEncode;
	x264Encode						m_x264Encode;
	TinyEvent						m_break;
};


#include "stdafx.h"
#include "EncodeTask.h"
#define ENCODE_FINISH_EVENT     TEXT("ENCODE_FINISH")

EncodeTask::EncodeTask(GraphicsCapture* pSys)
	:m_pSys(pSys)
{
}

BOOL EncodeTask::Submit()
{
	if (m_break.CreateEvent(FALSE, FALSE, ENCODE_FINISH_EVENT))
	{
		m_converter.Reset(new I420Converter(TinySize(m_cx, m_cy), TinySize(m_cx, m_cy)));
		m_x264Encode.Close();
		m_x264Encode.Open(m_cx, m_cy, param.GetRate());
		Closure s = BindCallback(&EncodeTask::MessagePump, this);
		return TinyTask::Submit(s);
	}
	return FALSE;
}


void EncodeTask::MessagePump()
{
	for (;;)
	{
		if (m_break.Lock(0))
		{
			break;
		}

	}
}

EncodeTask::~EncodeTask()
{

}

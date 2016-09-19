#include "stdafx.h"
#include "EncodePublishTask.h"


EncodePublishTask::EncodePublishTask(DXGraphics* graphics, MediaCapture* capture)
	:m_graphics(graphics),
	m_capture(capture)
{
	string str = GenerateGUID();
	if (!m_break.CreateEvent(FALSE, FALSE, str.c_str(), NULL))
		throw("create event error!");
	m_x264Done.Reset(new Delegate<void(BYTE*, INT, INT)>(this, &EncodePublishTask::x264Done));
	m_x264.EVENT_DONE += m_x264Done;
	m_aacDone.Reset(new Delegate<void(BYTE*, INT)>(this, &EncodePublishTask::aacDone));
	m_aac.EVENT_DONE += m_aacDone;
}

EncodePublishTask::~EncodePublishTask()
{
	m_x264.EVENT_DONE -= m_x264Done;
	m_aac.EVENT_DONE -= m_aacDone;
}

void EncodePublishTask::x264Done(BYTE* bits, INT size, INT type)
{
	DWORD dwTime = timeGetTime() - m_dwTime;
	switch (type)
	{
	case NAL_SPS:
		m_sps.resize(size);
		memcpy(&m_sps[0], bits, size);
		break;
	case NAL_PPS:
		m_pps.resize(size);
		memcpy(&m_pps[0], bits, size);
		m_client.SendSPSPPSPacket(m_pps, m_sps);
		break;
	default:
		m_client.SendVideoPacket(bits, size, dwTime);
		break;
	}
}
void EncodePublishTask::aacDone(BYTE* bits, INT size)
{
	DWORD dwTime = timeGetTime() - m_dwTime;
}

BOOL EncodePublishTask::Open(INT cx, INT cy, INT scaleX, INT scaleY, INT fps, INT rate, const WAVEFORMATEX& wfx)
{
	BOOL bRes = m_x264.Open(cx, cy, rate);
	if (!bRes)
		return FALSE;
	bRes = m_aac.Open(wfx);
	if (!bRes)
		return FALSE;
	m_converter.Reset(new I420Converter(TinySize(cx, cy), TinySize(scaleX, scaleY)));
	bRes = m_client.Connect("rtmp://10.121.86.127/live/test");
	if (!bRes)
		return FALSE;
	bRes = m_client.SendMetadataPacket(cx, cy, fps, rate);
	if (!bRes)
		return FALSE;

	return TRUE;
}
BOOL EncodePublishTask::Submit()
{
	m_dwTime = timeGetTime();
	Closure s = BindCallback(&EncodePublishTask::MessagePump, this);
	return TinyTaskBase::Submit(s);
}

void EncodePublishTask::Quit()
{
	m_break.SetEvent();
}

void EncodePublishTask::MessagePump()
{
	for (;;)
	{
		if (m_break.Lock(0))
			break;
		if (m_converter->BRGAToI420(m_graphics->GetPointer()))
			m_x264.Encode(m_converter->GetI420());
		m_aac.Encode(m_capture->GetAudioPointer());
		Sleep(30);
	}
}
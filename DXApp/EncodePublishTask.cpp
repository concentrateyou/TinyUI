#include "stdafx.h"
#include "EncodePublishTask.h"


EncodePublishTask::EncodePublishTask(DXGraphics* graphics, MediaCapture* capture)
	:m_graphics(graphics),
	m_capture(capture)
{
	string str = GenerateGUID();
	if (!m_event.CreateEvent(FALSE, FALSE, str.c_str(), NULL))
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
	DWORD dwTime = timeGetTime() - m_dwVideoTime;
	switch (type)
	{
	case NAL_SPS:
		m_sps.resize(size);
		memcpy(&m_sps[0], bits, size);
		break;
	case NAL_PPS:
		m_pps.resize(size);
		memcpy(&m_pps[0], bits, size);
		m_client.SendSPPacket(m_pps, m_sps, dwTime);
		break;
	default:
		m_client.SendVideoPacket(bits, size, dwTime);
		break;
	}
}
void EncodePublishTask::aacDone(BYTE* bits, INT size)
{
	//音视频交错 音频在视频后面 
	DWORD dwTime = timeGetTime();
	DWORD dwVideoTime = dwTime - m_dwVideoTime;
	DWORD dwAudioTime = dwTime - m_dwAudioTime;
	dwAudioTime += 9;
	m_client.SendAudioPacket(bits, size, dwAudioTime);
	TRACE("dwAudioTime:%d,dwVideoTime:%d\n", dwAudioTime, dwVideoTime);
}

BOOL EncodePublishTask::Open(INT cx, INT cy, INT scaleX, INT scaleY, INT frameRate, INT videoRate, const WAVEFORMATEX& wfx, INT audioRate)
{
	m_frameRate = frameRate;
	BOOL bRes = m_x264.Open(cx, cy, frameRate, videoRate);
	if (!bRes)
		return FALSE;
	bRes = m_aac.Open(wfx, audioRate);
	if (!bRes)
		return FALSE;
	m_converter.Reset(new I420Converter(TinySize(cx, cy), TinySize(scaleX, scaleY)));
	bRes = m_client.Connect("rtmp://10.121.86.127/live/test");
	if (!bRes)
		return FALSE;
	bRes = m_client.SendMetadataPacket(cx, cy, frameRate, videoRate, wfx, audioRate);
	if (!bRes)
		return FALSE;
	vector<BYTE> info;
	m_aac.GetSpecificInfo(info);
	bRes = m_client.SendAACPacket(&info[0], info.size());
	if (!bRes)
		return FALSE;
	return TRUE;
}
BOOL EncodePublishTask::Submit()
{
	m_dwVideoTime = timeGetTime();
	m_dwAudioTime = timeGetTime();
	Closure s = BindCallback(&EncodePublishTask::MessagePump, this);
	return TinyTaskBase::Submit(s);
}

void EncodePublishTask::Exit()
{
	m_event.SetEvent();
}

void EncodePublishTask::MessagePump()
{
	for (;;)
	{
		INT s = 1000 / m_frameRate;
		if (m_event.Lock(s))
			break;
		if (m_converter->BRGAToI420(m_graphics->GetPointer()))
		{
			m_x264.Encode(m_converter->GetI420());
		}
		m_aac.Encode(m_capture->GetAudioPointer());
	}
}
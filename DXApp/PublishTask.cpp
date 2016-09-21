#include "stdafx.h"
#include "PublishTask.h"


PublishTask::PublishTask(AudioEncodeTask* audioTask, VideoEncodeTask* videoTask)
	:m_audioTask(audioTask),
	m_videoTask(videoTask)
{

}


PublishTask::~PublishTask()
{
}

BOOL PublishTask::Connect()
{
	ASSERT(m_audioTask || m_videoTask);
	BOOL bRes = m_client.Connect("rtmp://10.121.86.127/live/test");
	if (!bRes)
		return FALSE;
	m_videoDone.Reset(new Delegate<void(BYTE*, INT, INT)>(this, &PublishTask::OnVideoDone));
	m_videoTask->GetEncode()->EVENT_DONE += m_videoDone;
	m_audioDone.Reset(new Delegate<void(BYTE*, INT)>(this, &PublishTask::OnAudioDone));
	m_audioTask->GetEncode()->EVENT_DONE += m_audioDone;

	return TRUE;
}

void PublishTask::OnVideoDone(BYTE* bits, INT size, INT type)
{

}

void PublishTask::OnAudioDone(BYTE* bits, INT size)
{

}

BOOL PublishTask::Submit()
{
	Closure s = BindCallback(&PublishTask::MessagePump, this);
	return TinyTaskBase::Submit(s);
}
void PublishTask::Exit()
{
	m_signal.SetEvent();
}

void PublishTask::MessagePump()
{
	for (;;)
	{

	}
}

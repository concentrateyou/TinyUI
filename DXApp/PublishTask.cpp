#include "stdafx.h"
#include "PublishTask.h"


PublishTask::PublishTask(AudioEncodeTask* audioTask, VideoEncodeTask* videoTask)
	:m_audioTask(audioTask),
	m_videoTask(videoTask)
{
	ASSERT(m_audioTask || m_videoTask);
	m_videoDone.Reset(new Delegate<void(TinyScopedReferencePtr<Sample>&)>(this, &PublishTask::OnVideoDone));
	m_videoTask->GetEncode()->EVENT_DONE += m_videoDone;
	m_audioDone.Reset(new Delegate<void(TinyScopedReferencePtr<Sample>&)>(this, &PublishTask::OnAudioDone));
	m_audioTask->GetEncode()->EVENT_DONE += m_audioDone;
}


PublishTask::~PublishTask()
{

}

BOOL PublishTask::Connect()
{
	ASSERT(m_audioTask || m_videoTask);
	/*BOOL bRes = m_client.Connect("rtmp://10.121.86.127/live/test");
	if (!bRes)
	return FALSE;*/

	return TRUE;
}

BOOL PublishTask::Submit()
{
	Closure s = BindCallback(&PublishTask::OnMessagePump, this);
	return TinyTaskBase::Submit(s);
}
void PublishTask::Exit()
{
	m_signal.SetEvent();
}

void PublishTask::OnExit()
{
	ASSERT(m_audioTask || m_videoTask);
	m_videoTask->GetEncode()->EVENT_DONE -= m_videoDone;
	m_audioTask->GetEncode()->EVENT_DONE -= m_audioDone;
}

void PublishTask::OnVideoDone(TinyScopedReferencePtr<Sample>& sample)
{
	/*TRACE("Video-PTS:%d\n", sample->PTS);*/
	//m_queue.Add(std::move(sample));
}

void PublishTask::OnAudioDone(TinyScopedReferencePtr<Sample>& sample)
{
	//TRACE("Audio-PTS:%d\n", sample->PTS);
	//m_queue.Add(std::move(sample));
}

void PublishTask::OnMessagePump()
{
	for (;;)
	{
		if (m_signal.Lock(3))
		{
			OnExit();
			break;
		}
	}
}

#pragma once
#include "AudioEncodeTask.h"
#include "VideoEncodeTask.h"
#include "RTMPClient.h"
#include "SampleQueue.h"
using namespace TinyUI::IO;
using namespace Media;

class PublishTask : public TinyTaskBase
{
public:
	PublishTask(AudioEncodeTask* audioTask, VideoEncodeTask* videoTask);
	~PublishTask();
	BOOL	Connect();
	BOOL	Submit();
	void	Exit() OVERRIDE;
private:
	void	OnMessagePump();
	void	OnExit();
	void	OnVideoDone(TinyScopedReferencePtr<Sample>& sample);
	void	OnAudioDone(TinyScopedReferencePtr<Sample>& sample);
private:
	RTMPClient			m_client;
	VideoEncodeTask*	m_videoTask;
	AudioEncodeTask*	m_audioTask;
	SampleQueue			m_queue;
	TinyScopedPtr<Delegate<void(TinyScopedReferencePtr<Sample>&)>>	m_videoDone;
	TinyScopedPtr<Delegate<void(TinyScopedReferencePtr<Sample>&)>>	m_audioDone;
};


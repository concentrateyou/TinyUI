#pragma once
#include "AudioEncodeTask.h"
#include "VideoEncodeTask.h"
#include "RTMPClient.h"
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
	void	MessagePump();
	void	OnVideoDone(BYTE* bits, INT size, INT type);
	void	OnAudioDone(BYTE* bits, INT size);
private:
	RTMPClient		 m_client;
	VideoEncodeTask* m_videoTask;
	AudioEncodeTask* m_audioTask;
	TinyScopedPtr<Delegate<void(BYTE*, INT, INT)>>	m_videoDone;
	TinyScopedPtr<Delegate<void(BYTE*, INT)>>		m_audioDone;
};


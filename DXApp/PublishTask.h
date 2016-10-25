#pragma once
#include "AudioEncode.h"
#include "VideoEncode.h"
#include "RTMPClient.h"
#include <queue>
using namespace TinyUI::IO;
using namespace Media;

class PublishTask : public TinyTaskBase
{
public:
	PublishTask(AudioEncode* audioTask, VideoEncode* videoTask);
	~PublishTask();
	BOOL	Connect();
	BOOL	Submit();
	BOOL	Close(DWORD dwMS) OVERRIDE;
private:
	void	Publish(Sample& sample);
	void	OnMessagePump();
	void	OnClose();
	void	OnVideoDone(BYTE*, LONG, const MediaTag&);
	void	OnAudioDone(BYTE*, LONG, const MediaTag&);
private:
	DWORD				m_baseTime;
	TinyLock			m_lock;
	RTMPClient			m_client;
	VideoEncode*		m_videoTask;
	AudioEncode*		m_audioTask;
	TinyEvent			m_close;
	vector<BYTE>		m_latestPPS;
	vector<BYTE>		m_latestSPS;
	std::queue<Sample>	m_samples;
	TinyScopedPtr<Delegate<void(BYTE*, LONG, const MediaTag&)>>	m_videoDone;
	TinyScopedPtr<Delegate<void(BYTE*, LONG, const MediaTag&)>>	m_audioDone;
};


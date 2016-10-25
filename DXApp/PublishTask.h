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
	void	OnMessagePump();
	void	OnClose();
	void	OnVideoDone(BYTE*, LONG, LONG, DWORD);
	void	OnAudioDone(BYTE*, LONG, LONG, DWORD);
private:
	RTMPClient		m_client;
	VideoEncode*	m_videoTask;
	AudioEncode*	m_audioTask;
	TinyEvent		m_close;
	vector<BYTE>	m_latestPPS;
	vector<BYTE>	m_latestSPS;
	std::queue<Sample>	m_samples;
	TinyLock			m_lock;
	DWORD				m_baseTime;
	TinyScopedPtr<Delegate<void(BYTE*, LONG, LONG, DWORD)>>	m_videoDone;
	TinyScopedPtr<Delegate<void(BYTE*, LONG, LONG, DWORD)>>	m_audioDone;
};


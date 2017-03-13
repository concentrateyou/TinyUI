#pragma once
#include "AudioEncode.h"
#include "VideoEncode.h"
#include "RTMPClient.h"
#include <concurrent_queue.h>
using namespace TinyUI::IO;
using namespace DShow;

namespace DXApp
{
	class PublishTask : public TinyTaskBase
	{
	public:
		PublishTask(AudioEncode* audioTask, VideoEncode* videoTask);
		~PublishTask();
		BOOL	Connect(const TinyString& url = "rtmp://10.121.86.127/live/test");
		BOOL	Submit();
		BOOL	Close(DWORD dwMS = INFINITE) OVERRIDE;
	private:
		void	Publish(Sample& sample);
		void	OnMessagePump();
		void	OnClose();
		void	OnVideoDone(BYTE*, LONG, const MediaTag&);
		void	OnAudioDone(BYTE*, LONG, const MediaTag&);
	private:
		DWORD				m_baseTime;
		RTMPClient			m_client;
		VideoEncode*		m_videoEncode;
		AudioEncode*		m_audioEncode;
		TinyEvent			m_close;
		vector<BYTE>		m_latestPPS;
		vector<BYTE>		m_latestSPS;
		concurrency::concurrent_queue<Sample>	m_samples;
		TinyScopedPtr<Delegate<void(BYTE*, LONG, const MediaTag&)>>	m_videoDone;
		TinyScopedPtr<Delegate<void(BYTE*, LONG, const MediaTag&)>>	m_audioDone;
	};
}


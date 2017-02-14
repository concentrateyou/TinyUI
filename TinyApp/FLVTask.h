#pragma once
#include "Common/TinyUtility.h"
#include "IO/TinyTaskBase.h"
#include "IO/TinyRingQueue.h"
#include "Common/TinyTime.h"
#include "FLVFile.h"
#include "FLVAudioTask.h"
using namespace TinyUI;
using namespace TinyUI::IO;
using namespace Decode;


class FLVTask : public TinyTaskBase
{
public:
	FLVTask(HWND hWND);
	~FLVTask();
	BOOL	Submit();
	BOOL	Close(DWORD dwMS) OVERRIDE;
private:
	void	OnMessagePump();
private:
	void	OnAudioDone(BYTE* bits, LONG size, FLV_PARAM& param);
	void	OnVideoDone(BYTE* bits, LONG size, FLV_PARAM& param);
public:
	TinyRingQueue			m_queue;
	TinyLock				m_lock;
private:
	TinyEvent				m_audio;
	TinyEvent				m_video;
	HWND					m_hWND;
	TinyPerformanceTimer	m_timer;
	Decode::FLVFile			m_flv;
	TinyScopedPtr<Delegate<void(BYTE*, LONG, FLV_PARAM&)>>	m_audioDone;
	TinyScopedPtr<Delegate<void(BYTE*, LONG, FLV_PARAM&)>>	m_videoDone;
	
	TinyScopedPtr<FLVAudioTask>	m_audioTask;
};



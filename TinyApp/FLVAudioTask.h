#pragma once
#include "Common/TinyUtility.h"
#include "IO/TinyTaskBase.h"
#include "IO/TinyRingQueue.h"
#include "Common/TinyTime.h"
#include "Media/TinySoundPlayer.h"
using namespace TinyUI;
using namespace TinyUI::IO;

class FLVTask;

class FLVAudioTask : public TinyTaskBase
{
public:
	FLVAudioTask(FLVTask* parentTask, HWND hWND, WAVEFORMATEX* pFMT);
	~FLVAudioTask();
	BOOL	Submit();
	BOOL	Close(DWORD dwMS) OVERRIDE;
private:
	void	OnMessagePump();
private:
	FLVTask*	m_parentTask;
	TinyEvent	m_close;
	TinyScopedPtr<BYTE> m_bits;
	Media::TinySoundPlayer	m_player;
};



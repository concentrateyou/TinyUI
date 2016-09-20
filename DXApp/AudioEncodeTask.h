#pragma once
#include "DXFramework.h"
#include "MediaCapture.h"
#include "aacEncode.h"
#include "RTMPClient.h"
using namespace TinyUI::IO;

class AudioEncodeTask : public TinyTaskBase
{
public:
	AudioEncodeTask(MediaCapture* capture);
	virtual ~AudioEncodeTask();
	BOOL		Open(const WAVEFORMATEX& wfx, DWORD dwAudioRate);
	BOOL		Submit();
	void		Exit() OVERRIDE;
	aacEncode*	GetEncode();
public:
	Event<void(BYTE*, INT)> EVENT_DONE;
private:
	void	MessagePump();
private:
	MediaCapture*								m_capture;
	aacEncode									m_aac;
	TinyScopedPtr<Delegate<void(BYTE*, INT)>>	m_aacDone;
};


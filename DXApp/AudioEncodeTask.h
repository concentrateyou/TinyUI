#pragma once
#include "DXFramework.h"
#include "AudioCapture.h"
#include "aacEncode.h"
#include "RTMPClient.h"
using namespace TinyUI::IO;
using namespace Media;

class AudioEncodeTask : public TinyTaskBase
{
public:
	AudioEncodeTask();
	virtual ~AudioEncodeTask();
	BOOL			Initialize(const AudioCapture::Name& name, const AudioCaptureParam& param);
	BOOL			Open(const WAVEFORMATEX& wfx, DWORD dwAudioRate);
	BOOL			Submit();
	void			Exit() OVERRIDE;
	aacEncode*		GetEncode();
	AudioCapture*	GetCapture();
private:
	void			OnMessagePump();
	void			OnExit();
	void			OnDone(BYTE* bits, INT size);
private:
	AudioCapture::Name							m_deviceName;
	AudioCaptureParam							m_audioParam;
	AudioCapture								m_audioCapture;
	aacEncode									m_aac;
};


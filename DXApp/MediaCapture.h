#pragma once
#include "VideoCapture.h"
#include "AudioCapture.h"

class MediaCapture
{
public:
	MediaCapture();
	~MediaCapture();
	BOOL	InitializeVideo(const VideoCapture::Name& name, const VideoCaptureParam& param);
	BOOL	InitializeAudio(const AudioCapture::Name& name, const AudioCaptureParam& param);
	BOOL	Start();
	BOOL	Stop();
	BOOL	Pause();
	BYTE*	GetVideoPointer() const;
	INT		GetVideoSize() const;
	BYTE*	GetAudioPointer() const;
	INT		GetAudioPointer() const;
private:
	VideoCapture		m_videoCapture;
	AudioCapture		m_audioCapture;
	AudioCapture::Name	m_audioName;
	VideoCapture::Name	m_videoName;
	AudioCaptureParam	m_audioParam;
	VideoCaptureParam	m_videoParam;
};


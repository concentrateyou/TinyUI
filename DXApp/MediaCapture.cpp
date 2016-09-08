#include "stdafx.h"
#include "MediaCapture.h"


MediaCapture::MediaCapture()
{
}


MediaCapture::~MediaCapture()
{
}

BOOL MediaCapture::InitializeVideo(const VideoCapture::Name& name, const VideoCaptureParam& param)
{
	m_videoName = name;
	m_videoParam = param;
	if (!m_videoCapture.Initialize(name))
		return FALSE;
	if (!m_videoCapture.Allocate(param))
		return FALSE;
	return TRUE;
}
BOOL MediaCapture::InitializeAudio(const AudioCapture::Name& name, const AudioCaptureParam& param)
{
	m_audioName = name;
	m_audioParam = param;
	if (!m_audioCapture.Initialize(name))
		return FALSE;
	if (!m_audioCapture.Allocate(param))
		return FALSE;
	return TRUE;
}

BOOL MediaCapture::Start()
{
	if (!m_videoCapture.Start())
		return FALSE;
	if (!m_audioCapture.Start())
		return FALSE;
	return TRUE;
}
BOOL MediaCapture::Stop()
{
	if (!m_videoCapture.Stop())
		return FALSE;
	if (!m_audioCapture.Stop())
		return FALSE;
	return TRUE;
}

BOOL MediaCapture::Pause()
{
	if (!m_videoCapture.Pause())
		return FALSE;
	if (!m_audioCapture.Pause())
		return FALSE;
	return TRUE;
}

BYTE* MediaCapture::GetVideoPointer() const
{
	return m_videoCapture.GetPointer();
}
INT	 MediaCapture::GetVideoSize() const
{
	return m_videoCapture.GetSize();
}
BYTE* MediaCapture::GetAudioPointer() const
{
	return m_audioCapture.GetPointer();
}
INT	MediaCapture::GetAudioPointer() const
{
	return m_audioCapture.GetSize();
}
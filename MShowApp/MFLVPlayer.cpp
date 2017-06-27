#include "stdafx.h"
#include "MFLVPlayer.h"

namespace MShow
{
	MFLVPlayer::MFLVPlayer(Callback<void(BYTE*, LONG)>&& callback)
		:m_callback(std::move(callback)),
		m_task(m_clock),
		m_audioTask(m_task, m_clock),
		m_audioRenderTask(m_audioTask, m_clock),
		m_videoTask(m_task, m_clock),
		m_videoRenderTask(m_videoTask, m_clock, BindCallback(&MFLVPlayer::OnVideo, this)),
		m_dwRate(25)
	{
	}

	MFLVPlayer::~MFLVPlayer()
	{
	}

	BOOL MFLVPlayer::Open(HWND hWND, LPCSTR pzURL)
	{
		m_szURL = pzURL;
		if (!m_task.Initialize(pzURL))
			return FALSE;
		if (!m_audioRenderTask.Initialize(hWND))
			return FALSE;
		if (!m_task.Submit())
			return FALSE;
		if (!m_videoTask.Submit())
			return FALSE;
		if (!m_videoRenderTask.Submit())
			return FALSE;
		if (!m_audioTask.Submit())
			return FALSE;
		if (!m_audioRenderTask.Submit())
			return FALSE;
		FLV_SCRIPTDATA s = m_task.GetScript();
		m_size.cx = static_cast<LONG>(s.width);
		m_size.cy = static_cast<LONG>(s.height);
		m_dwRate = static_cast<LONG>(s.videodatarate);
		return TRUE;
	}

	BOOL MFLVPlayer::SetVolume(LONG volume)
	{
		return m_audioRenderTask.SetVolume(volume);
	}

	void MFLVPlayer::OnVideo(BYTE* bits, LONG lsize)
	{
		if (!m_callback.IsNull())
		{
			m_callback(bits, lsize);
		}
	}

	BOOL MFLVPlayer::Close()
	{
		BOOL bRes = TRUE;
		if (m_task.IsValid())
			bRes &= m_task.Close(INFINITE);
		if (m_videoTask.IsValid())
			bRes &= m_videoTask.Close(INFINITE);
		if (m_videoRenderTask.IsValid())
			bRes &= m_videoRenderTask.Close(INFINITE);
		if (m_audioTask.IsValid())
			bRes &= m_audioTask.Close(INFINITE);
		if (m_audioRenderTask.IsValid())
			bRes &= m_audioRenderTask.Close(INFINITE);
		m_clock.SetBasePTS(-1);
		m_clock.SetBaseTime(-1);
		Sleep(100);
		return bRes;
	}

	TinySize MFLVPlayer::GetSize() const
	{
		return m_size;
	}

	DWORD MFLVPlayer::GetRate() const
	{
		return m_dwRate;
	}

	TinyString	MFLVPlayer::GetURL() const
	{
		return m_szURL;
	}
}


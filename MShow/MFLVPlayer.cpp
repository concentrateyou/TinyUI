#include "stdafx.h"
#include "MFLVPlayer.h"

namespace MShow
{
	MFLVPlayer::MFLVPlayer()
		:m_task(m_clock, m_msgqueue),
		m_audioTask(m_task, m_clock, m_msgqueue),
		m_audioRenderTask(m_audioTask, m_clock, m_msgqueue),
		m_videoTask(m_task, m_clock, m_msgqueue),
		m_videoRenderTask(m_videoTask, m_clock, m_msgqueue),
		m_dwRate(25),
		m_bBreak(FALSE)
	{
		m_msgqueue.SetCallback(BindCallback(&MFLVPlayer::OnMessage, this));
	}

	MFLVPlayer::MFLVPlayer(Callback<void(BYTE*, LONG)>&& audioCB)
		:m_task(m_clock, m_msgqueue),
		m_audioTask(m_task, m_clock, m_msgqueue),
		m_audioRenderTask(m_audioTask, m_clock, m_msgqueue, std::move(audioCB)),
		m_videoTask(m_task, m_clock, m_msgqueue),
		m_videoRenderTask(m_videoTask, m_clock, m_msgqueue),
		m_dwRate(25),
		m_bBreak(FALSE)
	{
		m_msgqueue.SetCallback(BindCallback(&MFLVPlayer::OnMessage, this));
	}

	MFLVPlayer::~MFLVPlayer()
	{
		m_msgqueue.Close();
	}

	void MFLVPlayer::SetErrorCallback(TinyUI::Callback<void(INT)>&& callback)
	{
		m_task.SetErrorCallback(std::move(callback));
	}

	BOOL MFLVPlayer::Open(HWND hWND, LPCSTR pzURL)
	{
		m_hWND = hWND;
		m_szURL = pzURL;
		if (!m_task.Initialize(m_szURL.STR()))
			return FALSE;
		if (!m_audioRenderTask.Initialize())
			return FALSE;
		if (!m_videoTask.Submit())
			return FALSE;
		if (!m_videoRenderTask.Initialize(hWND))
			return FALSE;
		if (!m_videoRenderTask.Submit())
			return FALSE;
		if (!m_audioTask.Submit())
			return FALSE;
		if (!m_audioRenderTask.Submit())
			return FALSE;
		if (!m_task.Submit())
			return FALSE;
		FLV_SCRIPTDATA s = m_task.GetScript();
		m_size.cx = static_cast<LONG>(s.width);
		m_size.cy = static_cast<LONG>(s.height);
		m_dwRate = static_cast<LONG>(s.videodatarate);
		return TRUE;
	}

	BOOL MFLVPlayer::SetVolume(DWORD volume)
	{
		return m_audioRenderTask.SetVolume(volume);
	}

	BOOL MFLVPlayer::Close()
	{
		BOOL bRes = TRUE;
		if (m_task.IsActive())
			bRes &= m_task.Close(INFINITE);
		if (m_videoRenderTask.IsActive())
			bRes &= m_videoRenderTask.Close(INFINITE);
		if (m_videoTask.IsActive())
			bRes &= m_videoTask.Close(INFINITE);
		if (m_audioRenderTask.IsActive())
			bRes &= m_audioRenderTask.Close(INFINITE);
		if (m_audioTask.IsActive())
			bRes &= m_audioTask.Close(INFINITE);
		m_audioTask.GetAudioQueue().RemoveAll();
		m_videoTask.GetVideoQueue().RemoveAll();
		m_task.GetVideoQueue().RemoveAll();
		m_task.GetAudioQueue().RemoveAll();
		m_clock.SetBasePTS(INVALID_TIME);
		m_clock.SetBaseTime(INVALID_TIME);
		Sleep(100);
		return bRes;
	}

	void MFLVPlayer::OnMessage(UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (msg == WM_FLV_PARSE_FAIL)
		{
			TRACE("WM_FLV_PARSE_FAIL");
			this->Close();
			if (!this->Open(m_hWND, m_szURL.CSTR()))
			{
				TRACE("WM_FLV_PARSE_FAIL Reopen FAIL\n");
				LOG(ERROR) << "WM_FLV_PARSE_FAIL Reopen FAIL";
			}
			else
			{
				TRACE("WM_FLV_PARSE_FAIL Reopen OK\n");
				LOG(INFO) << "WM_FLV_PARSE_FAIL Reopen OK";
			}
		}
		if (msg == WM_VIDEO_X264_DECODE_FAIL)
		{
			TRACE("WM_VIDEO_X264_DECODE_FAIL\n");
			this->Close();
			if (!this->Open(m_hWND, m_szURL.CSTR()))
			{
				TRACE("WM_VIDEO_X264_DECODE_FAIL Reopen FAIL\n");
				LOG(ERROR) << "WM_VIDEO_X264_DECODE_FAIL Reopen FAIL";
			}
			else
			{
				TRACE("WM_VIDEO_X264_DECODE_FAIL Reopen OK\n");
				LOG(INFO) << "WM_VIDEO_X264_DECODE_FAIL Reopen OK";
			}
		}
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

	WAVEFORMATEX* MFLVPlayer::GetFormat()
	{
		return m_audioTask.GetFormat();
	}

	LONGLONG MFLVPlayer::GetBasePTS()
	{
		return m_task.GetBasePTS();
	}
}


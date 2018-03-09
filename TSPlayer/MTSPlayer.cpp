#include "stdafx.h"
#include "MTSPlayer.h"

namespace TSPlayer
{
	MTSPlayer::MTSPlayer()
		:m_task(m_clock, m_msgqueue),
		m_audioTask(m_task, m_clock, m_msgqueue),
		m_audioRenderTask(m_audioTask, m_clock, m_msgqueue),
		m_videoTask(m_task, m_clock, m_msgqueue),
		m_videoRenderTask(m_videoTask, m_clock, m_msgqueue),
		m_dwRate(25),
		m_bBreak(FALSE),
		m_bPlaying(FALSE)
	{
		m_msgqueue.SetCallback(BindCallback(&MTSPlayer::OnMessage, this));
	}

	MTSPlayer::MTSPlayer(Callback<void(BYTE*, LONG)>&& audioCB)
		:m_task(m_clock, m_msgqueue),
		m_audioTask(m_task, m_clock, m_msgqueue),
		m_audioRenderTask(m_audioTask, m_clock, m_msgqueue, std::move(audioCB)),
		m_videoTask(m_task, m_clock, m_msgqueue),
		m_videoRenderTask(m_videoTask, m_clock, m_msgqueue),
		m_dwRate(25),
		m_bBreak(FALSE),
		m_bPlaying(FALSE)
	{
		m_msgqueue.SetCallback(BindCallback(&MTSPlayer::OnMessage, this));
	}

	MTSPlayer::~MTSPlayer()
	{
		m_msgqueue.Close();
	}
	BOOL MTSPlayer::IsPlaying() const
	{
		return m_bPlaying;
	}
	BOOL MTSPlayer::Open(HWND hWND, LPCSTR pzURL)
	{
		m_hWND = hWND;
		m_szURL = pzURL;
		if (!m_task.Initialize(m_szURL.STR(), BindCallback(&MTSPlayer::OnError, this)))
			return FALSE;
		if (!m_audioRenderTask.Initialize())
			return FALSE;
		if (!m_videoRenderTask.Initialize(hWND))
			return FALSE;
		if (!m_videoTask.Submit())
			return FALSE;
		if (!m_videoRenderTask.Submit())
			return FALSE;
		if (!m_audioTask.Submit())
			return FALSE;
		if (!m_audioRenderTask.Submit())
			return FALSE;
		if (!m_task.Submit())
			return FALSE;
		return TRUE;
	}

	BOOL MTSPlayer::SetVolume(DWORD volume)
	{
		return m_audioRenderTask.SetVolume(volume);
	}

	BOOL MTSPlayer::Close()
	{
		BOOL bRes = TRUE;
		LOG(INFO) << "[MFLVPlayer] Close SetCurrentAudioTS 0";
		if (m_task.IsActive())
			bRes &= m_task.Close(INFINITE);
		LOG(INFO) << "MFLVTask Close:" << bRes;
		if (m_videoRenderTask.IsActive())
			bRes &= m_videoRenderTask.Close(INFINITE);
		LOG(INFO) << "MVideoRenderTask Close:" << bRes;
		if (m_videoTask.IsActive())
			bRes &= m_videoTask.Close(INFINITE);
		LOG(INFO) << "MVideoTask Close:" << bRes;
		if (m_audioRenderTask.IsActive())
			bRes &= m_audioRenderTask.Close(INFINITE);
		LOG(INFO) << "MAudioRenderTask Close:" << bRes;
		if (m_audioTask.IsActive())
			bRes &= m_audioTask.Close(INFINITE);
		LOG(INFO) << "MAudioRenderTask Close:" << bRes;
		m_audioTask.GetAudioQueue().RemoveAll();
		m_videoTask.GetVideoQueue().RemoveAll();
		m_task.GetVideoQueue().RemoveAll();
		m_task.GetAudioQueue().RemoveAll();
		m_clock.SetClock(INVALID_TIME);
		Sleep(300);
		return bRes;
	}

	void MTSPlayer::OnMessage(UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (msg == WM_FLV_PARSE_FAIL)
		{
			TRACE("WM_FLV_PARSE_FAIL");
			this->Close();
			LOG(INFO) << "[OnMessage] Close OK\n\n\n\n\n";
			if (this->Open(m_hWND, m_szURL.CSTR()))
			{
				TRACE("WM_FLV_PARSE_FAIL Reopen OK\n");
				LOG(INFO) << "WM_FLV_PARSE_FAIL Reopen OK";
			}
			else
			{
				TRACE("WM_FLV_PARSE_FAIL Reopen FAIL\n");
				LOG(ERROR) << "WM_FLV_PARSE_FAIL Reopen FAIL";
			}
		}
		if (msg == WM_VIDEO_X264_DECODE_FAIL)
		{
			TRACE("WM_VIDEO_X264_DECODE_FAIL\n");
			this->Close();
			LOG(INFO) << "[OnMessage] Close OK\n\n\n\n\n";
			if (this->Open(m_hWND, m_szURL.CSTR()))
			{
				TRACE("WM_VIDEO_X264_DECODE_FAIL Reopen OK\n");
				LOG(INFO) << "WM_VIDEO_X264_DECODE_FAIL Reopen OK";
			}
			else
			{
				TRACE("WM_VIDEO_X264_DECODE_FAIL Reopen FAIL\n");
				LOG(ERROR) << "WM_VIDEO_X264_DECODE_FAIL Reopen FAIL";
			}
		}
		if (msg == WM_PLAY_RESUME)
		{
			TRACE("[MFLVPlayer] WM_PLAY_RESUME Close Timer\n");
			LOG(INFO) << "[MFLVPlayer] WM_PLAY_RESUME Close Timer";
			m_retryTimer.Close();
		}
	}

	void MTSPlayer::OnError(INT iError)
	{
		switch (iError)
		{
		case WSAETIMEDOUT:
		case WSAENETDOWN:
		case WSAENETUNREACH:
		case WSAENETRESET:
		case WSAECONNABORTED:
		case WSAECONNRESET:
		{
			m_retryTimer.Close();
			m_retryTimer.SetCallback(5000, BindCallback(&MTSPlayer::OnTry, this));//Ã¿¸ô5ÃëÖØÊÔ
			TRACE("[MFLVPlayer] OnError:%d\n", iError);
			LOG(ERROR) << "[MFLVPlayer] OnError:" << iError;
		}
		break;
		}
	}

	void MTSPlayer::OnTry()
	{
		if (!this->IsPlaying())
		{
			this->Close();
			if (this->Open(m_hWND, m_szURL.CSTR()))
			{
				TRACE("[MFLVPlayer] OnTry Open OK\n");
				LOG(INFO) << "[MFLVPlayer] OnTry Open OK";
				MSG msg = { 0 };
				msg.message = WM_PLAY_RESUME;
				m_msgqueue.PostMsg(msg);
			}
			else
			{
				TRACE("OnTry Open FAIL\n");
				LOG(ERROR) << "OnTry Open FAIL";
			}
		}
	}

	TinySize MTSPlayer::GetSize() const
	{
		return m_size;
	}

	DWORD MTSPlayer::GetRate() const
	{
		return m_dwRate;
	}

	TinyString	MTSPlayer::GetURL() const
	{
		return m_szURL;
	}

	WAVEFORMATEX* MTSPlayer::GetFormat()
	{
		return m_audioTask.GetFormat();
	}
}


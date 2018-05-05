#pragma once
#include "MAudioTask.h"
#include "MAudioRenderTask.h"
#include "MVideoTask.h"
#include "MVideoRenderTask.h"

namespace FLVPlayer
{
	/// <summary>
	/// FLV²¥·ÅÆ÷
	/// </summary>
	class MFLVPlayer
	{
		DISALLOW_COPY_AND_ASSIGN(MFLVPlayer)
	public:
		MFLVPlayer();
		MFLVPlayer(Callback<void(BYTE*, LONG)>&& audioCB);
		virtual ~MFLVPlayer();
		BOOL			IsPlaying() const;
		BOOL			Open(HWND hWND, LPCSTR pzURL);
		BOOL			Close();
		BOOL			SetVolume(DWORD volume);
		TinySize		GetSize() const;
		DWORD			GetRate() const;
		TinyString		GetURL() const;
		WAVEFORMATEX*	GetFormat();
	private:
		void			OnMessage(UINT, WPARAM, LPARAM);
		void			OnError(INT iError);
		void			OnTry();
	private:
		BOOL							m_bBreak;
		BOOL							m_bPlaying;
		DWORD							m_dwRate;
		HWND							m_hWND;
		TinyString						m_szURL;
		TinySize						m_size;
		TinyMsgQueue					m_msgqueue;
		TinySimpleTaskTimer				m_retryTimer;
		MClock							m_clock;
		MFLVTask						m_task;
		MAudioTask						m_audioTask;
		MVideoTask						m_videoTask;
		MAudioRenderTask				m_audioRenderTask;
		MVideoRenderTask				m_videoRenderTask;
	};
}



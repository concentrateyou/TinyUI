#pragma once
#include "MAudioTask.h"
#include "MAudioRenderTask.h"
#include "MVideoTask.h"
#include "MVideoRenderTask.h"

namespace MShow
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
		BOOL			Open(HWND hWND, LPCSTR pzURL);
		BOOL			Close();
		BOOL			SetVolume(DWORD volume);
		TinySize		GetSize() const;
		DWORD			GetRate() const;
		TinyString		GetURL() const;
		WAVEFORMATEX*	GetFormat();
		LONGLONG		GetBasePTS();
		void			SetErrorCallback(TinyUI::Callback<void(INT)>&& callback);
	private:
		void			OnMessage(UINT, WPARAM, LPARAM);
	private:
		BOOL							m_bBreak;
		DWORD							m_dwRate;
		HWND							m_hWND;
		TinyString						m_szURL;
		TinySize						m_size;
		TinyMsgQueue					m_msgqueue;
		MClock							m_clock;
		MFLVTask						m_task;
		MAudioTask						m_audioTask;
		MVideoTask						m_videoTask;
		MAudioRenderTask				m_audioRenderTask;
		MVideoRenderTask				m_videoRenderTask;
	};
}



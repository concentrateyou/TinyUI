#pragma once
#include "MAudioTask.h"
#include "MAudioRenderTask.h"
#include "MVideoTask.h"
#include "MVideoRenderTask.h"

namespace MShow
{
	/// <summary>
	/// FLV������
	/// </summary>
	class MFLVPlayer
	{
		DISALLOW_COPY_AND_ASSIGN(MFLVPlayer)
	public:
		MFLVPlayer(Callback<void(BYTE*, LONG)>&& videoCB);
		MFLVPlayer(Callback<void(BYTE*, LONG)>&& audioCB, Callback<void(BYTE*, LONG)>&& videoCB);
		virtual ~MFLVPlayer();
		BOOL			Open(HWND hWND, LPCSTR pzURL);
		BOOL			Close();
		BOOL			SetVolume(LONG volume);
		TinySize		GetSize() const;
		DWORD			GetRate() const;
		TinyString		GetURL() const;
		WAVEFORMATEX*	GetFormat();
		LONGLONG		GetBasePTS();
	private:
		void			OnAudio(BYTE* bits, LONG lsize);
		void			OnVideo(BYTE* bits, LONG size);
	private:
		DWORD							m_dwRate;
		TinyString						m_szURL;
		TinySize						m_size;
		MClock							m_clock;
		MFLVTask						m_task;
		MAudioTask						m_audioTask;
		MVideoTask						m_videoTask;
		MAudioRenderTask				m_audioRenderTask;
		MVideoRenderTask				m_videoRenderTask;
		Callback<void(BYTE*, LONG)>		m_audioCB;
		Callback<void(BYTE*, LONG)>		m_videoCB;
	};
}



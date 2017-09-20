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
		MFLVPlayer(Callback<void(BYTE*, LONG)>&& videoCopyCB, Closure&& videoRenderCB);
		MFLVPlayer(Callback<void(BYTE*, LONG)>&& audioCB, Callback<void(BYTE*, LONG)>&& videoCopyCB, Closure&& videoRenderCB);
		virtual ~MFLVPlayer();
		BOOL			Open(HWND hWND, LPCSTR pzURL);
		BOOL			Close();
		BOOL			SetVolume(DWORD volume);
		TinySize		GetSize() const;
		DWORD			GetRate() const;
		TinyString		GetURL() const;
		WAVEFORMATEX*	GetFormat();
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
	};
}



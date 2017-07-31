#pragma once
#include "MShowCommon.h"
#include "MPlayView.h"
#include "MFLVPlayer.h"
using namespace TinyUI;

namespace MShow
{
	class MPlayView;
	/// <summary>
	/// ²¥·Å
	/// </summary>
	class MPlayController
	{
		friend class MShowController;
		DISALLOW_COPY_AND_ASSIGN(MPlayController)
	public:
		MPlayController(MPlayView& view);
		virtual ~MPlayController();
		BOOL	Initialize();
		BOOL	Open(LPCSTR pzURL);
		BOOL	Close();
	private:
		void	OnVideo(BYTE* bits, LONG size);
		void	OnVolume(DWORD pos);
	private:
		MPlayView&		m_view;
		DX11Graphics2D	m_graphics;
		DX11Image2D		m_video2D;
		MFLVPlayer		m_player;
		TinyScopedPtr<Delegate<void(DWORD)>>	m_onVolume;
	};
}



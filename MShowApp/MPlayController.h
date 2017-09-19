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
	class MPlayController : public TinyTaskBase
	{
		friend class MShowController;
		DISALLOW_COPY_AND_ASSIGN(MPlayController)
	public:
		MPlayController(MPlayView& view);
		virtual ~MPlayController();
		BOOL	Initialize();
		BOOL	Submit();
		BOOL	Close(DWORD dwMS) OVERRIDE;
	private:
		void	OnMessagePump();
	private:
		BOOL			m_bBreak;
		MPlayView&		m_view;
		DX11Graphics2D	m_graphics;
		DX11Image2D		m_video2D;
	};
}



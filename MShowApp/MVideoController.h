#pragma once
#include "MVideoView.h"
#include "MFLVPlayer.h"
using namespace DXFramework;

namespace MShow
{
	class MVideoDialog : public TinyCustomDialog
	{
	public:
		MVideoDialog();
		virtual ~MVideoDialog();
		LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	public:
		TinyString GetAddress();
	private:
		TinyTextBox	m_textbox;
		TinyString	m_address;
	};

	class MVideoController
	{
		DISALLOW_COPY_AND_ASSIGN(MVideoController)
	public:
		MVideoController(MVideoView& view);
		virtual ~MVideoController();
		BOOL	Initialize();
		BOOL	Open(LPCSTR pzURL);
		BOOL	Close();
		HANDLE	GetHandle();//¹²ÏíµÄÎÆÀí
	private:
		void	OnVideo(BYTE* bits, LONG size);
	private:
		MVideoView&		m_view;
		MFLVPlayer		m_player;
		DX11Graphics2D	m_graphics;
		DX11Image2D		m_video2D;
		DX11Image2D		m_copy2D;
	};
}



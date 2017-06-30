#pragma once
#include "Control/TinyMenu.h"
#include "MVideoView.h"
#include "MFLVPlayer.h"
using namespace DXFramework;
using namespace TinyUI::IO;

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
	class MShowController;
	class MPreviewController;
	class MRTMPEncoder;
	class MVideoElement;
	/// <summary>
	/// 音视频控制器
	/// </summary>
	class MVideoController
	{
		friend class MRTMPEncoder;
		friend class MPreviewController;
		friend class MShowController;
		DISALLOW_COPY_AND_ASSIGN(MVideoController)
	public:
		MVideoController(MVideoView& view);
		virtual ~MVideoController();
		BOOL			Initialize();
		BOOL			Open(LPCSTR pzURL);
		BOOL			Close();
		HANDLE			GetHandle();//共享的纹理
		TinyString		GetURL() const;
		WAVEFORMATEX*	GetFormat();
	private:
		void	OnAdd();
		void	OnRemove();
		void	OnAudio(BYTE* bits, LONG size);
		void	OnVideo(BYTE* bits, LONG size);
		void	OnMenuClick(void*, INT wID);
		void	OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		void	OnLButtonDBClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		void	OnVolume(DWORD volume);
	private:
		TinyMenu		m_popup;
		MVideoView&		m_view;
		DX11Graphics2D	m_graphics;
		DX11Image2D		m_video2D;
		DX11Image2D		m_copy2D;
		MFLVPlayer		m_player;
		TinyEvent		m_signal;
		MVideoElement*	m_pVideo;
	private:
		TinyScopedPtr<Delegate<void(DWORD)>>						m_onVolume;
		TinyScopedPtr<Delegate<void(void*, INT)>>					m_onMenuClick;
		TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>>	m_onLButtonDBClick;
		TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>>	m_onRButtonDown;
	};
}



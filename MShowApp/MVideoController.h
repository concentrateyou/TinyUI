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
	class MRTMPEncoder;
	class MVideoElement;
	class MShowController;
	class MPreviewController;
	/// <summary>
	/// ����Ƶ������
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
		HANDLE			GetHandle();//���������
		TinyString		GetURL() const;
		WAVEFORMATEX*	GetFormat();
		DX11Element2D*	GetElement();
		void			AddElement();
	private:
		void	OnAdd();
		void	OnRemove();
		void	OnChange();
	private:
		void	OnAudio(BYTE* bits, LONG size);
		void	OnVideo(BYTE* bits, LONG size);
		void	OnMenuClick(void*, INT wID);
		void	OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		void	OnLButtonDBClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		void	OnVolume(DWORD volume);
	public:
		Event<void(BYTE*, LONG)> EVENT_AUDIO;
	private:
		TinyLock					m_lock;
		TinyMenu					m_popup;
		TinyEvent					m_synchronize;
		MVideoElement*				m_pVideo;
		MVideoView&					m_view;
		DX11Graphics2D				m_graphics;
		DX11Image2D					m_video2D;
		TinyScopedPtr<MFLVPlayer>	m_player;
	private:
		TinyScopedPtr<Delegate<void(DWORD)>>						m_onVolume;
		TinyScopedPtr<Delegate<void(void*, INT)>>					m_onMenuClick;
		TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>>	m_onLButtonDBClick;
		TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>>	m_onRButtonDown;
	};
}



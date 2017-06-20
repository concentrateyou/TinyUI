#pragma once
#include "Control/TinyCustomDialog.h"
#include "Control/TinyTextBox.h"
#include "MPreviewController.h"
#include "MFLVPlayer.h"
#include "MFLVModel.h"
using namespace DXFramework;

namespace MShow
{
	class MPreviewController;

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
	/// <summary>
	/// 视频源View
	/// </summary>
	class VideoView : public TinyControl
	{
		DISALLOW_COPY_AND_ASSIGN(VideoView)
	public:
		VideoView(MPreviewController& controller);
		virtual ~VideoView();
		//5个创建函数
		DWORD	RetrieveStyle() OVERRIDE;
		DWORD	RetrieveExStyle() OVERRIDE;
		LPCSTR	RetrieveClassName() OVERRIDE;
		LPCSTR	RetrieveTitle() OVERRIDE;
		HICON	RetrieveIcon() OVERRIDE;
		BOOL	Create(HWND hParent, INT x, INT y, INT cx, INT cy);
		DX2D&	GetD2D();
		//事件
		LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnLButtonDBClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	private:
		void	DrawView();
		void	OnVideo(BYTE* bits, LONG size);
		void	OnAdd();
		void	OnRemove();
		void	OnMenuClick(void*, INT wID);
	private:
		DX2D						m_dx2d;
		TinyMenu					m_menu;
		MFLVPlayer					m_player;
		MPreviewController&			m_controller;
		TinyScopedPtr<MFLVModel>	m_model;
		TinyScopedPtr<Delegate<void(void*, INT)>>	m_onMenuClick;
	};
}



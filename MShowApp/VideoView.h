#pragma once
#include "Control/TinyCustomDialog.h"
#include "Control/TinyTextBox.h"
#include "MPreviewController.h"
#include "MFLVPlayer.h"
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
		VideoView();
		virtual ~VideoView();
		//5个创建函数
		DWORD RetrieveStyle() OVERRIDE;
		DWORD RetrieveExStyle() OVERRIDE;
		LPCSTR RetrieveClassName() OVERRIDE;
		LPCSTR RetrieveTitle() OVERRIDE;
		HICON RetrieveIcon() OVERRIDE;
		BOOL Create(HWND hParent, INT x, INT y, INT cx, INT cy);
		//事件
		LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnLButtonDBClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	public:
		DX2D& GetD2D();
		void SetController(MPreviewController* pController);
	private:
		void OnVideo(ID2D1Bitmap1* bitmap);
	private:
		DX2D				m_d2d;
		MFLVPlayer			m_player;
		MPreviewController*	m_pController;
	};
}



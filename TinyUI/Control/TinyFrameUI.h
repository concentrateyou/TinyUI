#pragma once
#include "TinyControl.h"
#include "TinyButton.h"
#include "../Render/TinyImage.h"
#include "TinyListBox.h"

namespace TinyUI
{
	//系统参数
#define CAPTION_HEIGHT			32
#define CLIENT_MARGIN_LEFT		2
#define CLIENT_MARGIN_TOP		2
#define CLIENT_MARGIN_RIGHT		2
#define CLIENT_MARGIN_BOTTOM	2
#define BUTTON_SIZE_CX			30
#define BUTTON_SIZE_CY			30
	/// <summary>
	/// 窗口框架,无窗口
	/// </summary>
	class TinyFrameUI : public TinyControl
	{
		DECLARE_DYNAMIC(TinyFrameUI)
	public:
		TinyFrameUI();
		virtual ~TinyFrameUI();
		//5个创建函数
		virtual DWORD RetrieveStyle();
		virtual DWORD RetrieveExStyle();
		virtual LPCSTR RetrieveClassName();
		virtual LPCSTR RetrieveTitle();
		virtual HICON RetrieveIcon();
		BOOL Create(HWND hParent, INT x, INT y, INT cx, INT cy);
		//事件
		virtual LRESULT OnNCLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnNCLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnNCLButtonDBClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnNCRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnNCRButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnNCRButtonDBClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnNCHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	public:
		BOOL ShowWindow(INT nCmdShow) throw();
		BOOL UpdateWindow() throw();
		void CenterWindow(HWND parent, HWND window, TinySize pref) throw();
	protected:
		//缓冲的样式
		TinySize m_size;
	};
}

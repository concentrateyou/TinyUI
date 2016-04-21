#pragma once
#include "../Control/TinyControl.h"
#include "../Render/TinyImage.h"

namespace TinyUI
{
	/// <summary>
	/// 皮肤窗口
	/// </summary>
	class TinySkinWindow : public TinyControl
	{
		DECLARE_DYNAMIC(TinySkinWindow)
	public:
		TinySkinWindow();
		virtual ~TinySkinWindow();
		//5个创建函数
		DWORD RetrieveStyle() OVERRIDE;
		DWORD RetrieveExStyle() OVERRIDE;
		LPCSTR RetrieveClassName() OVERRIDE;
		LPCSTR RetrieveTitle() OVERRIDE;
		HICON RetrieveIcon() OVERRIDE;
		//事件
		LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnNCPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnNCCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnNCHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		//方法
		BOOL Create(HWND hParent, INT x, INT y, INT cx, INT cy);
	private:
		//标题栏高度
		INT GetCaptionCY();
	protected:
		//缓冲的样式
		TinySize	m_size;
	};
}

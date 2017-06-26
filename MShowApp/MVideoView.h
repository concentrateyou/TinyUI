#pragma once
#include "Control/TinyCustomDialog.h"
#include "Control/TinyTextBox.h"
#include "MShowCommon.h"
using namespace DXFramework;

namespace MShow
{
	/// <summary>
	/// 视频源View
	/// </summary>
	class MVideoView : public TinyControl
	{
		DISALLOW_COPY_AND_ASSIGN(MVideoView)
	public:
		MVideoView();
		virtual ~MVideoView();
		//5个创建函数
		DWORD	RetrieveStyle() OVERRIDE;
		DWORD	RetrieveExStyle() OVERRIDE;
		LPCSTR	RetrieveClassName() OVERRIDE;
		LPCSTR	RetrieveTitle() OVERRIDE;
		HICON	RetrieveIcon() OVERRIDE;
		BOOL	Create(HWND hParent, INT x, INT y, INT cx, INT cy);
		//事件
		LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	};
}



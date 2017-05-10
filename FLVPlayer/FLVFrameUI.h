#pragma once
#include "Control/TinyControl.h"
#include "FLVAudio.h"
#include "FLVVideo.h"
#include "FLVDecodeTask.h"
using namespace TinyUI;

namespace FLVPlayer
{
	class FLVFrameUI : public TinyControl
	{
	public:
		FLVFrameUI();
		virtual ~FLVFrameUI();
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
	private:
		TinyScopedPtr<FLVAudio>	m_audioTask;
		TinyScopedPtr<FLVVideo>	m_videoTask;

		TinyScopedPtr<FLVDecodeTask>	m_task;
	};
}




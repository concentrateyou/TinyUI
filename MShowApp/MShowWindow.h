#pragma once
#include "MShowCommon.h"
#include "MTabView.h"
#include "MVolumeView.h"
#include "MVideoView.h"
#include "MImageView.h"
#include "MPreviewView.h"
#include "MShadowView.h"
#include "MPlayView.h"
#include "MAudioView.h"
using namespace TinyUI;

namespace MShow
{
	class MShowController;
	/// <summary>
	/// 主窗口UI
	/// </summary>
	class MShowWindow : public TinyControl
	{
		friend class MShowController;
		DECLARE_DYNAMIC(MShowWindow)
	public:
		MShowWindow();
		virtual ~MShowWindow();
		DWORD RetrieveStyle() OVERRIDE;
		DWORD RetrieveExStyle() OVERRIDE;
		LPCSTR RetrieveClassName() OVERRIDE;
		LPCSTR RetrieveTitle() OVERRIDE;
		HICON RetrieveIcon() OVERRIDE;
		LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	public:
		BOOL Create(HWND hParent, INT x, INT y, INT cx, INT cy);
	private:
		void OnTabChange(void*);
		void OnToggle(void*, INT);
		void OnPusher(void*, INT);
	private:
		MTabView				m_tabViews[3];
		MVolumeView				m_volumeViews[6];
		MVideoView				m_videoViews[6];
		MImageView				m_imageViews[6];
		MAudioView				m_audioViews[6];
		MPreviewView			m_previewView;
		//MShadowView				m_shadowView;//影子窗口
		MPlayView				m_playView;
		MVolumeView				m_playVolumeView;
		TinyButton				m_layout[3];
		TinyButton				m_toggle;
		TinyButton				m_pusher;
		TinyTabControl			m_tab;
	private:
		TinyScopedPtr<Delegate<void(void*)>>		m_onTabChange;
	};
}
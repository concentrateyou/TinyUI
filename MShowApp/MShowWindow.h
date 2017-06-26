#pragma once
#include "MShowCommon.h"
#include "MTabView.h"
#include "MVolumeView.h"
#include "MVideoView.h"
#include "MVideoController.h"
using namespace TinyUI;

namespace MShow
{
	class MShowController;
	/// <summary>
	/// Ö÷´°¿ÚUI
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
	private:
		MTabView				m_tabViews[2];
		MVolumeView				m_volumeViews[6];
		MVideoView				m_videoViews[6];
		TinyTabControl			m_tab;
	private:
		TinyScopedPtr<Delegate<void(void*)>>	m_onTabChange;
	};
}
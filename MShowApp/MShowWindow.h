#pragma once
#include "MPreviewView.h"
#include "MPreviewController.h"
#include "MAudioRenderTask.h"
#include "Control/TinyTabControl.h"
#include "TabView.h"
#include "MVideoView.h"
#include "MImageView.h"
#include "VolumeView.h"
using namespace TinyUI;

namespace MShow
{
	/// <summary>
	/// Ö÷´°¿ÚUI
	/// </summary>
	class MShowWindow : public TinyControl
	{
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
		MPreviewView& GetPreivewView();
	private:
		void OnTabChange(void*);
	private:
		TabView					m_tabViews[2];
		VolumeView				m_volumeViews[6];
		TinyTabControl			m_tab;
		MPreviewView			m_previewView;
		MPreviewController		m_controller;
		TinyArray<MVideoView*>	m_videoViews;
		TinyArray<MImageView*>	m_imageViews;
	private:
		TinyScopedPtr<Delegate<void(void*)>>	m_onTabChange;
	};
}




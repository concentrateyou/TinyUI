#pragma once
#include "MShowCommon.h"
#include "MPreviewView.h"
#include "Control/TinyLabel.h"
#include "Control/TinyTextBox.h"
#include "Control/TinyButton.h"
#include "Control/TinyIPAddress.h"
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
		void OnDraw();
	private:
		MPreviewView		m_previewView;
		TinyTextBox			m_txtPreviewURL;
		TinyButton			m_btnPreview;
		TinyButton			m_btnRecord;
		TinyLabel			m_analyserBAR;
		TinyIPAddress		m_address;
	private:
		TinyScopedPtr<Delegate<void(void*)>>		m_onTabChange;
	};
}
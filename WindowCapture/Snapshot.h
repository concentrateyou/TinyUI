#pragma once
#include "Control/TinyControl.h"
#include "Render/TinyRectTracker.h"
using namespace TinyUI;

namespace WindowCapture
{
	class Snapshot : public TinyControl, public TinyRectTracker
	{
		DECLARE_DYNAMIC(Snapshot)
	public:
		Snapshot();
		virtual ~Snapshot();
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
		LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		void OnChangedRect(const TinyRectangle& rectOld) OVERRIDE;
	public:
		BOOL Create(HWND hParent);
		BOOL Initialize();
		void Uninitialize();
	private:
		HBITMAP			m_hBBitmap;
		HBITMAP			m_hFBitmap;
		HBITMAP			m_hOldBBitmap;
		HBITMAP			m_hOldFBitmap;
		HDC				m_hBDC;
		HDC				m_hFDC;
	};
}



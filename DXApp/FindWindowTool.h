#pragma once
#include "Control/TinyLabel.h"
#include "Utility.h"
using namespace TinyUI;

namespace DXApp
{
	class FindWindowTool : public TinyLabel
	{
	public:
		FindWindowTool();
		virtual ~FindWindowTool();
		LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnLButtonDBClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		void LoadResource();
		void ShowTool(BOOL bShow);
	private:
		HWND		m_lastHWND;
		BOOL		m_bDragging;
		BOOL		m_bDisplay;
		TinyBitmap	m_bitmapDrag1;
		TinyBitmap	m_bitmapDrag2;
		HCURSOR		m_hCursor;
		HCURSOR		m_hOldCursor;
		TinyPoint	m_lastPos;
	public:
		Event<void(HWND)> EVENT_WINDOWCHANGE;
	};
}



#pragma once
#include "Control/TinyLabel.h"
#include "Utility.h"
using namespace TinyUI;

class FindTool : public TinyLabel
{
public:
	FindTool();
	virtual ~FindTool();
	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	LRESULT OnLButtonDBClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	LRESULT OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	LRESULT OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	void Load();
	void ShowParent(BOOL bShow);
	HWND GetHWND();
private:
	HWND		m_currentHWND;
	BOOL		m_bDragging;
	BOOL		m_bDisplay;
	TinyBitmap	m_bitmapDrag1;
	TinyBitmap	m_bitmapDrag2;
	HCURSOR		m_hCursor;
	HCURSOR		m_hOldCursor;
	TinyPoint	m_lastPos;
};



#pragma once
#include "Control/TinyLabel.h"
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
public:
	BOOL	BeginDrag();
private:
	BOOL		m_bDragging;
	TinyBitmap	m_bitmapDrag1;
	TinyBitmap	m_bitmapDrag2;
	HCURSOR		m_hCursor;
};



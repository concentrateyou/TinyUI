#pragma once
#include "Control/TinyControl.h"
#include "GraphicsCapture.h"
using namespace DXFramework;

class DXWindow : public TinyControl
{
	DECLARE_DYNAMIC(DXWindow)
public:
	DXWindow();
	virtual ~DXWindow();
	DWORD RetrieveStyle() OVERRIDE;
	DWORD RetrieveExStyle() OVERRIDE;
	LPCSTR RetrieveClassName() OVERRIDE;
	LPCSTR RetrieveTitle() OVERRIDE;
	HICON RetrieveIcon() OVERRIDE;
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	LRESULT OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	LRESULT OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	BOOL Create(HWND hParent, INT x, INT y, INT cx, INT cy);
private:
	TinyEvent		m_close;
	TinyEvent		m_render;
	GraphicsCapture	m_graphics;
};


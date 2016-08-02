#pragma once
#include "Control/TinyControl.h"
#include "D3D/TinyD3D10Device.h"
using namespace TinyUI;

class CMainFrame : public TinyControl
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();
	~CMainFrame();
public:
	DWORD RetrieveStyle() OVERRIDE;
	DWORD RetrieveExStyle() OVERRIDE;
	LPCSTR RetrieveClassName() OVERRIDE;
	LPCSTR RetrieveTitle() OVERRIDE;
	HICON RetrieveIcon() OVERRIDE;
	BOOL Create(HWND hParent, INT x, INT y, INT cx, INT cy);
public:
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	LRESULT OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
private:
	TinyUI::D3D::TinyD3D10Device m_d3d10Device;
};


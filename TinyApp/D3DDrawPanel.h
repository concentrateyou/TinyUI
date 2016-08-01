#pragma once
#include "Control/TinyControl.h"
#pragma comment(lib,"TinyUI.lib")
using namespace TinyUI;

/// <summary>
/// ªÊ÷∆√Ê∞Â
/// </summary>
class CD3DDrawPanel : public TinyControl
{
	DECLARE_DYNAMIC(CD3DDrawPanel)
public:
	CD3DDrawPanel();
	~CD3DDrawPanel();

	virtual BOOL Create(HWND hParent, INT x, INT y, INT cx, INT cy);

public:
	LPCSTR RetrieveClassName() OVERRIDE;
	LPCSTR RetrieveTitle() OVERRIDE;
	HICON RetrieveIcon() OVERRIDE;
	DWORD RetrieveStyle() OVERRIDE;
	DWORD RetrieveExStyle() OVERRIDE;
public:
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;

};


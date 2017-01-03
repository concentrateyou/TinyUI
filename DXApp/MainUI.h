#pragma once
#include "Control/TinyControl.h"
#include "Control/TinyButton.h"
using namespace TinyUI;

class MainUI : public TinyControl
{
	DECLARE_DYNAMIC(MainUI)
public:
	MainUI();
	virtual ~MainUI();
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
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;

public:
	BOOL Create(HWND hParent, INT x, INT y, INT cx, INT cy);
private:
	void BuildUI();
	void Resize(INT cx, INT cy);
private:
	TinyButton m_broadcast;
	TinyButton m_recording;

	TinyButton m_game;
	TinyButton m_screen;
	TinyButton m_window;
	TinyButton m_capture;
	TinyButton m_text;
	TinyButton m_image;



};
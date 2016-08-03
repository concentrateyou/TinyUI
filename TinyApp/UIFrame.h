#pragma once
#include "Common/TinyModule.h"
#include "Windowless/TinyVisualHWND.h"
#include "Common/TinyLogging.h"
#include "Common/TinyHook.h"
#include "Common/TinyEvent.h"
#include "Control/TinyLabel.h"
#include "Control/TinyButton.h"
#include "Windowless/TinyVisualRichText.h"
using namespace TinyUI;

class CUIFrame : public TinyControl
{
	DECLARE_DYNAMIC(CUIFrame)
public:
	CUIFrame();
	virtual ~CUIFrame();
	//5个创建函数
	DWORD RetrieveStyle() OVERRIDE;
	DWORD RetrieveExStyle() OVERRIDE;
	LPCSTR RetrieveClassName() OVERRIDE;
	LPCSTR RetrieveTitle() OVERRIDE;
	HICON RetrieveIcon() OVERRIDE;
	//事件
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	LRESULT OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	LRESULT OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	//方法
	BOOL Create(HWND hParent, INT x, INT y, INT cx, INT cy);
};


#pragma once
#include "Common/TinyModule.h"
#include "Windowless/TinyVisualHWND.h"
#include "Common/TinyLogging.h"
#include "Common/TinyHook.h"
#include "Common/TinyEvent.h"
#include "Control/TinyLabel.h"
#include "Control/TinyButton.h"
#include "Control/TinyRichTextBox.h"
#include "Common/TinyEvent.h"
#include "D3DDrawPanel.h"
#include <algorithm>
#include <map>
#include <functional>

class CMainFrame : public TinyControl
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();
	virtual ~CMainFrame();
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
private:
	void OnInjectLibrary(void*, INT);
protected:
	TinyButton			m_inject;
	TinyLabel			m_lblState;
	CD3DDrawPanel		m_drawCtrl;
	CHARFORMAT2			m_cf;
	TinyScopedPtr<Delegate<void(void*, INT)>> m_onInjectClick;
};


#pragma once
#include "Common/TinyModule.h"
#include "Windowless/TinyVisualHWND.h"
#include "Control/TinyComboBox.h"
#include "Control/TinyButton.h"
#include "Control/TinyLabel.h"
#include "Control/TinyButton.h"
#include "Control/TinyLabel.h"
#include "Control/TinyRichTextBox.h"

#include <sapi.h>
#include <sphelper.h>
#pragma comment(lib,"sapi.lib")

using namespace TinyUI;

class ChatFrame : public TinyControl
{
	DECLARE_DYNAMIC(ChatFrame)
public:
	ChatFrame();
	virtual ~ChatFrame();
	//5个创建函数
	DWORD RetrieveStyle() OVERRIDE;
	DWORD RetrieveExStyle() OVERRIDE;
	LPCSTR RetrieveClassName() OVERRIDE;
	LPCSTR RetrieveTitle() OVERRIDE;
	HICON RetrieveIcon() OVERRIDE;
	BOOL Create(HWND hParent, INT x, INT y, INT cx, INT cy);
	//事件
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	LRESULT OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	LRESULT OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
public:
	void OnSpeek(void*, INT);
private:
	TinyRichTextBox		m_textbox;
	TinyButton			m_speek;
	TinyScopedPtr<Delegate<void(void*, INT)>> m_onSpeek;

	TinyComPtr<ISpVoice> m_spVoice;
};


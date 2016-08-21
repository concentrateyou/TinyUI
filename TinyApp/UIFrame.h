#pragma once
#include "Common/TinyModule.h"
#include "Windowless/TinyVisualHWND.h"
#include "Control/TinyComboBox.h"
#include "Control/TinyButton.h"
#include "Control/TinyLabel.h"
#include "VideoCapture.h"
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
	void OnSelectChange1(INT index);
	void OnSelectChange2(INT index);
	void OnStart(void*, INT);
	void OnStop(void*, INT);
private:
	TinyLabel			m_control;
	Media::VideoCapture m_device;
	TinyComboBox		m_device1;
	TinyScopedPtr<Delegate<void(INT)>> m_onChange1;
	TinyComboBox		m_device2;
	TinyScopedPtr<Delegate<void(INT)>> m_onChange2;
	TinyButton			m_start;
	TinyScopedPtr<Delegate<void(void*, INT)>> m_onStart;
	TinyButton			m_stop;
	TinyScopedPtr<Delegate<void(void*, INT)>> m_onStop;
	vector<Media::VideoCapture::Name> m_names;
	vector<Media::VideoCaptureParam>  m_params;
};


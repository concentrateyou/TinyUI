#pragma once
#include "Common/TinyModule.h"
#include "Windowless/TinyVisualHWND.h"
#include "Control/TinyComboBox.h"
#include "Control/TinyButton.h"
#include "Control/TinyLabel.h"
#include "VideoCapture.h"
#include "AudioCapture.h"
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
	void OnVideoSelectChange1(INT index);
	void OnVideoSelectChange2(INT index);
	void OnVideoStart(void*, INT);
	void OnVideoStop(void*, INT);

	void OnAudioSelectChange1(INT index);
	void OnAudioSelectChange2(INT index);
	void OnAudioStart(void*, INT);
	void OnAudioStop(void*, INT);
private:
	TinyLabel			m_control;
	Media::VideoCapture m_videoDevice;
	TinyComboBox		m_videoDevice1;
	TinyScopedPtr<Delegate<void(INT)>> m_onVideoChange1;
	TinyComboBox		m_videoDevice2;
	TinyScopedPtr<Delegate<void(INT)>> m_onVideoChange2;
	TinyButton			m_videoStart;
	TinyScopedPtr<Delegate<void(void*, INT)>> m_onVideoStart;
	TinyButton			m_videoStop;
	TinyScopedPtr<Delegate<void(void*, INT)>> m_onVideoStop;
	vector<Media::VideoCapture::Name> m_videoNames;
	vector<Media::VideoCaptureParam>  m_videoParams;

	Media::AudioCapture m_audioDevice;
	vector<Media::AudioCapture::Name> m_audioNames;
	vector<Media::AudioCaptureParam>  m_audioParams;
	TinyScopedPtr<Delegate<void(INT)>> m_onAudioChange1;
	TinyScopedPtr<Delegate<void(INT)>> m_onAudioChange2;
	TinyScopedPtr<Delegate<void(void*, INT)>> m_onAudioStart;
	TinyScopedPtr<Delegate<void(void*, INT)>> m_onAudioStop;
	TinyComboBox		m_audioDevice1;
	TinyComboBox		m_audioDevice2;
	TinyButton			m_audioStart;
	TinyButton			m_audioStop;
};


#pragma once
#include "Common/TinyModule.h"
#include "Windowless/TinyVisualHWND.h"
#include "Control/TinyComboBox.h"
#include "Control/TinyButton.h"
#include "Control/TinyLabel.h"
#include "Media/TinyWASAPIAudioCapture.h"
#include "VideoCapture.h"
#include "AudioCapture.h"
#include "SoundPlayer.h"
#include "RenderTask.h"
using namespace TinyUI;

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
	void OnVideoSelectChange1(INT index);
	void OnVideoSelectChange2(INT index);
	void OnVideoStart(void*, INT);
	void OnVideoStop(void*, INT);

	void OnAudioSelectChange1(INT index);
	void OnAudioSelectChange2(INT index);
	void OnAudioStart(void*, INT);
	void OnAudioStop(void*, INT);

private:

	Media::TinyWASAPIAudioCapture m_capture;

	TinyLabel			m_control;
	DShow::VideoCapture m_videoDevice;
	TinyComboBox		m_videoDevice1;
	TinyScopedPtr<Delegate<void(INT)>> m_onVideoChange1;
	TinyComboBox		m_videoDevice2;
	TinyScopedPtr<Delegate<void(INT)>> m_onVideoChange2;
	TinyButton			m_videoStart;
	TinyScopedPtr<Delegate<void(void*, INT)>> m_onVideoStart;
	TinyButton			m_videoStop;
	TinyScopedPtr<Delegate<void(void*, INT)>> m_onVideoStop;
	vector<DShow::VideoCapture::Name> m_videoNames;
	vector<DShow::VideoCaptureParam>  m_videoParams;

	DShow::AudioCapture m_audioDevice;
	vector<DShow::AudioCapture::Name> m_audioNames;
	vector<DShow::AudioCaptureParam>  m_audioParams;

	TinyScopedPtr<RenderTask>		m_renderTask;

	TinyScopedPtr<Delegate<void(INT)>> m_onAudioChange1;
	TinyScopedPtr<Delegate<void(INT)>> m_onAudioChange2;
	TinyScopedPtr<Delegate<void(void*, INT)>> m_onAudioStart;
	TinyScopedPtr<Delegate<void(void*, INT)>> m_onAudioStop;
	TinyComboBox		m_audioDevice1;
	TinyComboBox		m_audioDevice2;
	TinyButton			m_audioStart;
	TinyButton			m_audioStop;

};


#pragma once
#include "Common/TinyModule.h"
#include "Windowless/TinyVisualHWND.h"
#include "Control/TinyComboBox.h"
#include "Control/TinyButton.h"
#include "Control/TinyLabel.h"
#include "Control/TinyRichTextBox.h"
#include "VideoCapture.h"
#include "AudioCapture.h"
#include "AudioCapture.h"
#include "RenderTask.h"
#include "Media/TinyWave.h"

#include "Snapshot.h"

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

	void OnAudioInputSelectChange1(INT index);
	void OnAudioInputSelectChange2(INT index);
	void OnAudioInputStart(void*, INT);
	void OnAudioInputStop(void*, INT);
	void OnAudioInput(BYTE* bits, LONG size, FLOAT ts, LPVOID ps);

	void OnAudioOutputSelectChange1(INT index);
	void OnAudioOutputSelectChange2(INT index);
	void OnAudioOutputStart(void*, INT);
	void OnAudioOutputStop(void*, INT);
	void OnAudioOutput(BYTE* bits, LONG size, FLOAT ts, LPVOID ps);

private:

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

	TinyScopedPtr<RenderTask>		m_renderTask;

	DShow::AudioCapture m_audioInput;
	vector<DShow::AudioCapture::Name> m_audioInputNames;
	vector<DShow::AudioCaptureParam>  m_audioInputParams;
	TinyScopedPtr<Delegate<void(INT)>> m_onAudioInputChange1;
	TinyScopedPtr<Delegate<void(INT)>> m_onAudioInputChange2;
	TinyScopedPtr<Delegate<void(void*, INT)>> m_onAudioInputStart;
	TinyScopedPtr<Delegate<void(void*, INT)>> m_onAudioInputStop;
	TinyComboBox		m_audioInput1;
	TinyComboBox		m_audioInput2;
	TinyButton			m_audioInputStart;
	TinyButton			m_audioInputStop;

	DShow::AudioCapture m_audioOutput;
	vector<DShow::AudioCapture::Name> m_audioOutputNames;
	vector<DShow::AudioCaptureParam>  m_audioOutputParams;
	TinyScopedPtr<Delegate<void(INT)>> m_onAudioOutputChange1;
	TinyScopedPtr<Delegate<void(INT)>> m_onAudioOutputChange2;
	TinyScopedPtr<Delegate<void(void*, INT)>> m_onAudioOutputStart;
	TinyScopedPtr<Delegate<void(void*, INT)>> m_onAudioOutputStop;

	TinyComboBox		m_audioOutput1;
	TinyComboBox		m_audioOutput2;
	TinyButton			m_audioOutputStart;
	TinyButton			m_audioOutputStop;

	Media::TinyWaveFile		m_waveFile;

	WindowCapture::Snapshot m_snapshot;
};


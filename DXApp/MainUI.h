#pragma once
#include "Control/TinyControl.h"
#include "Control/TinyButton.h"
#include "Control/TinyTrackBar.h"
#include "Control/TinyLabel.h"
#include "DXWindow.h"
#include "VideoScene.h"
#include "GameScene.h"
#include "ImageScene.h"
#include "WindowScene.h"
#include "ScreenScene.h"
#include "TextScene.h"
#include "Snapshot.h"
using namespace TinyUI;

namespace DXApp
{
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
		BOOL Create(HWND hParent, INT x, INT y, INT cx, INT cy, DXWindow* pDXWND);
		LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	private:
		void CreateUI();
		void DestoryUI();
		void Resize(INT cx, INT cy);
		void OnBroadcastClick(void*, INT);
		void OnRecordClick(void*, INT);
		void OnGameClick(void*, INT);
		void OnScreenClick(void*, INT);
		void OnWindowClick(void*, INT);
		void OnCaptureClick(void*, INT);
		void OnTextClick(void*, INT);
		void OnImageClick(void*, INT);
		void OnAudioClick(void*, INT);
		void OnSelected(RECT);
		void OnContextChange(void*);
		void OnMicrophoneVolumeChange(void*, INT);
	private:
		TinyButton m_broadcast;
		TinyButton m_record;
		TinyButton m_game;
		TinyButton m_screen;
		TinyButton m_window;
		TinyButton m_capture;
		TinyButton m_text;
		TinyButton m_image;
		TinyButton m_audio;
		TinyLabel	m_lblSpeaker;
		TinyLabel	m_lblMicrophone;
		TinyTrackBar m_speaker;
		TinyTrackBar m_microphone;
		TinyScopedPtr<Delegate<void(void*, INT)>>	m_onBroadcastClick;
		TinyScopedPtr<Delegate<void(void*, INT)>>	m_onRecordClick;
		TinyScopedPtr<Delegate<void(void*, INT)>>	m_onGameClick;
		TinyScopedPtr<Delegate<void(void*, INT)>>	m_onScreenClick;
		TinyScopedPtr<Delegate<void(void*, INT)>>	m_onWindowClick;
		TinyScopedPtr<Delegate<void(void*, INT)>>	m_onCaptureClick;
		TinyScopedPtr<Delegate<void(void*, INT)>>	m_onTextClick;
		TinyScopedPtr<Delegate<void(void*, INT)>>	m_onImageClick;
		TinyScopedPtr<Delegate<void(void*, INT)>>	m_onAudioClick;
		TinyScopedPtr<Delegate<void(void*)>>		m_onTextChange;

		DXWindow* m_pDXWND;

		Snapshot	m_snapshot;
		TinyScopedPtr<Delegate<void(RECT)>> m_onSelected;

		TextScene	m_textScene;
		GameScene	m_gameScene;
		VideoScene	m_videoScene;
		ImageScene	m_imageScene;
		WindowScene m_windowScene;
		ScreenScene	m_screenScene;

		DShow::AudioCapture m_audioCapture;
		TinyScopedPtr<Delegate<void(void*,INT)>> m_onMicrophoneVolumeChange;

		RenderTask	m_renderTask;
	};
}
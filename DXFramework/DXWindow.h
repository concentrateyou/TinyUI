#pragma once
#include "DX11.h"
#include "DXCamera.h"
#include "DX11Image.h"
#include "DX11TextureShader.h"
#include "VideoCapture.h"
#include "DX11CaptureTask.h"

namespace DXFramework
{
	class DXWindow : public TinyControl
	{
		DECLARE_DYNAMIC(DXWindow)
	public:
		DXWindow();
		virtual ~DXWindow();
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
		BOOL Create(HWND hParent, INT x, INT y, INT cx, INT cy);
	public:
		BOOL Render();
	private:
		void OnVideo(const BYTE* pBits, INT size, LPVOID lpParamer);
	private:
		BOOL					m_success;
		DX11					m_dx11;
		DX11Image				m_dxVideo;
		DX11Image				m_dxGame;
		DXCamera				m_camera;
		DX11TextureShader		m_textureShader;
		Media::VideoCapture		m_videoCapture;
		Callback<void(const BYTE*, INT, LPVOID)> m_videoCB;
		DX11CaptureTask			m_captureTask;
	};
}



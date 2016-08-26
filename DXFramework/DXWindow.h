#pragma once
#include "DX10.h"
#include "DXCamera.h"
#include "DX10Image.h"
#include "DX10TextureShader.h"
#include "VideoCapture.h"

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
		DX10					m_dx10;
		DX10Image				m_dxVideo;
		DX10Image				m_dxGame;
		DXCamera				m_camera;
		DX10TextureShader		m_textureShader;
		Media::VideoCapture		m_videoCapture;
		Callback<void(const BYTE*, INT, LPVOID)> m_videoCB;
		IO::TinySharedMemory	m_memory;
	};
}



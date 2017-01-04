#pragma once
#include "DXFramework.h"
#include "DX11Texture.h"
#include "DX11Image.h"
using namespace TinyUI::IO;

namespace DXFramework
{
	class DX11CaptureTask : public TinyTaskBase
	{
	public:
		DX11CaptureTask(DX11* pDX11, INT cx, INT cy);
		DX11CaptureTask(DX11* pDX11);
		virtual ~DX11CaptureTask();
		DX11Image&				GetTexture();
		BOOL					Submit();
		BOOL					Close(DWORD dwMS = INFINITE) OVERRIDE;
	private:
		void					MessagePump();
		WNDINFO					GetWNDINFO();
		SharedCaptureDATA*		GetSharedCapture();
		void					Tick();
		BOOL					BeginCapture();
		BOOL					EndCapture();
		BOOL					AttemptCapture(const TinyString& className, const TinyString& exeName, const TinyString& dllName);
		DWORD					BuildEvents();
		static BOOL CALLBACK	EnumWindow(HWND hwnd, LPARAM lParam);
	private:
		INT						m_cx;
		INT						m_cy;
		BOOL					m_bCapturing;
		DX11*					m_pDX11;
		TinyEvent				m_captureStart;
		TinyEvent				m_captureStop;
		TinyEvent				m_captureReady;
		TinyEvent				m_captureExit;
		TinyEvent				m_close;
		WNDINFO					m_targetWND;
		SharedTexture			m_texture;
		TinySharedMemory		m_memory;
	};
}


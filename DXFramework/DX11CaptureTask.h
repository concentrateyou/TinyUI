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
		DX11CaptureTask(DX11* pDX11, DX11Image& image);
		virtual ~DX11CaptureTask();
		void					SetConfig(const TinyString& className, const TinyString& exeName, const TinyString& dllName);
		BOOL					Submit();
		BOOL					Close(DWORD dwMS = INFINITE) OVERRIDE;
	private:
		void					MessagePump();
		WNDINFO					GetWNDINFO();
		SharedCaptureDATA*		GetSharedCaptureDATA();
		SharedTextureDATA*		GetSharedTextureDATA();
		void					Tick();
		BOOL					BeginCapture();
		BOOL					EndCapture();
		BOOL					AttemptCapture(const TinyString& className, const TinyString& exeName, const TinyString& dllName);
		DWORD					BuildEvents();
		static BOOL CALLBACK	EnumWindow(HWND hwnd, LPARAM lParam);
	private:
		TinySize				m_size;
		BOOL					m_bCapturing;
		DX11*					m_pDX11;
		TinyEvent				m_captureStart;
		TinyEvent				m_captureStop;
		TinyEvent				m_captureReady;
		TinyEvent				m_captureExit;
		TinyEvent				m_close;
		WNDINFO					m_targetWND;
		TinyString				m_className;
		TinyString				m_exeName;
		TinyString				m_dllName;
		DX11Image&				m_image;
		TinySharedMemory		m_memory;
		TinySharedMemory		m_textureMemery;
	};
}


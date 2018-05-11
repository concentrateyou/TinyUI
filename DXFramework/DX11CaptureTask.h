#pragma once
#include "DXFramework.h"
#include "DX11Texture2D.h"
#include "DX11Image2D.h"
using namespace TinyUI::IO;

namespace DXFramework
{
	class DX11CaptureTask : public TinyThread
	{
	public:
		DX11CaptureTask(DX11* pDX11, DX11Image2D& image);
		virtual ~DX11CaptureTask();
		void					SetConfig(const TinyString& className, const TinyString& exeName, const TinyString& dllName);
		BOOL					Submit();
		BOOL					Close(DWORD dwMS = INFINITE) OVERRIDE;
		SharedCaptureDATA*		GetSharedCaptureDATA();
		SharedTextureDATA*		GetSharedTextureDATA(DWORD dwSize);
		BYTE*					GetSharedTexture(DWORD dwSize);
	private:
		void					OnMessagePump();
		WNDINFO					GetWNDINFO();
		void					Tick();
		BOOL					BeginCapture();
		BOOL					EndCapture();
		BOOL					AttemptCapture(const TinyString& className, const TinyString& exeName, const TinyString& dllName);
		BOOL					CreateEvents();
		BOOL					OpenEvents();
		void					CloseEvents();
		static BOOL CALLBACK	EnumWindow(HWND hwnd, LPARAM lParam);
	private:
		TinySize				m_size;
		BOOL					m_bCapturing;
		DX11*					m_pDX11;
		TinyLock				m_lock;
		WNDINFO					m_targetWND;
		TinyEvent				m_start;
		TinyEvent				m_stop;
		TinyEvent				m_ready;
		TinyEvent				m_exit;
		TinyEvent				m_close;
		TinyString				m_className;
		TinyString				m_exeName;
		TinyString				m_dllName;
		DX11Image2D&			m_image;
		TinySharedMemory		m_captureMemory;
		TinySharedMemory		m_textureMemery;
	};
}


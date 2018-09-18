#pragma once
#include "DXFramework.h"
#include "DX11Texture2D.h"
#include "DX11Image2D.h"
using namespace TinyFramework::IO;

namespace DXFramework
{
	class DX11CaptureRunner : public TinyThread
	{
	public:
		DX11CaptureRunner(DX11* pDX11, DX11Image2D& image);
		virtual ~DX11CaptureRunner();
		void					SetConfig(const TinyString& className, const TinyString& exeName, const TinyString& dllName);
		BOOL					Submit();
		BOOL					Close(DWORD dwMS = INFINITE) OVERRIDE;
		HookDATA*				GetHookDATA();
		SharedTextureDATA*		GetSharedTextureDATA(DWORD dwSize);
		BYTE*					GetSharedTexture(DWORD dwSize);
		BOOL					IsCapturing() const;
	private:
		void					OnMessagePump();
		WNDINFO					GetWNDINFO();
		void					Tick();
		BOOL					BeginCapture();
		BOOL					EndCapture();
		BOOL					Detour(const TinyString& className, const TinyString& exeName, const TinyString& dllName, BOOL bSafe = TRUE);
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


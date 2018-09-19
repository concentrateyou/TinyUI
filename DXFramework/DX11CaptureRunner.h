#pragma once
#include "DXFramework.h"
#include "DX11Texture2D.h"
#include "DX11Image2D.h"
using namespace TinyFramework::IO;

namespace DXFramework
{
	class GameInject
	{
		DISALLOW_COPY_AND_ASSIGN(GameInject)
	public:
		GameInject();
		~GameInject();
		BOOL Open(const WNDINFO& ws, BOOL anticheat);
		BOOL Wait(DWORD dwMS);
		BOOL GetExitCode(DWORD& exit);
		void Close();
	private:
		TinyProcess	m_process;
	};

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
		void					Tick();
		void					OnMessagePump();
		WNDINFO					GetWNDINFO();
		BOOL					Start();
		void					Stop();
		BOOL					Detour(const TinyString& className, const TinyString& exeName, const TinyString& dllName, BOOL bSafe = TRUE);
		BOOL					OpenEvents();
		void					CloseEvents();
		static BOOL CALLBACK	EnumWindow(HWND hwnd, LPARAM lParam);
	private:
		TinySize				m_size;
		BOOL					m_bCapturing;
		BOOL					m_bActive;
		DX11*					m_pDX11;
		TinyLock				m_lock;
		WNDINFO					m_targetWND;
		TinyEvent				m_start;
		TinyEvent				m_stop;
		TinyEvent				m_init;
		TinyEvent				m_ready;
		TinyEvent				m_close;
		TinyString				m_szClass;
		TinyString				m_szEXE;
		TinyString				m_szDLL;
		GameInject				m_inject;
		DX11Image2D&			m_image2D;
		TinySharedMemory		m_hookDATA;
		TinySharedMemory		m_textureDATA;
	};
}


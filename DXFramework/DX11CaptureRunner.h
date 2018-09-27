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
		BOOL Close();
	private:
		TinyProcess	m_process;
	};

	class DX11CaptureRunner
	{
	public:
		DX11CaptureRunner(DX11* pDX11, DX11Image2D& image);
		virtual ~DX11CaptureRunner();
		void					SetConfig(const TinyString& className, const TinyString& exeName);
		HookDATA*				GetHookDATA();
		TextureDATA*			GetTextureDATA();
		BOOL					IsCapturing() const;
		BOOL					Tick(INT64& timestamp);
		void					Close();
	private:
		BOOL					InitializeDATA();
		WNDINFO					GetWNDINFO();
		BOOL					StartCapture();
		void					StopCapture();
		BOOL					CopyTextureDATA();
		BOOL					AttemptExisting();
		BOOL					AttemptDetour(BOOL anticheat);
		BOOL					Detour(const TinyString& className, const TinyString& exeName, const TinyString& dllName, BOOL bSafe = TRUE);
		BOOL					OpenEvents();
		void					CloseEvents();
		static BOOL CALLBACK	EnumWindow(HWND hwnd, LPARAM lParam);
	private:
		volatile BOOL			m_bCapturing;
		volatile BOOL			m_bActive;
		DX11*					m_pDX11;
		TinyLock				m_lock;
		WNDINFO					m_target;
		TinySize				m_size;
		TinyEvent				m_start;
		TinyEvent				m_stop;
		TinyEvent				m_sourceReady;
		TinyEvent				m_targetReady;
		TinyString				m_szClass;
		TinyString				m_szEXE;
		TinyString				m_szDLL;
		GameInject				m_injector;
		BYTE*					m_textures[2];
		TinyMutex				m_mutes[2];
		DX11Image2D&			m_image2D;
		TinySharedMemory		m_hookDATA;
		TinySharedMemory		m_textureDATA;
	};
}


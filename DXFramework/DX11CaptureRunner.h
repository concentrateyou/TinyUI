#pragma once
#include "DXFramework.h"
#include "DX11Texture2D.h"
#include "DX11Image2D.h"
using namespace TinyFramework::IO;

namespace DXFramework
{
	class DX11CaptureRunner
	{
		DISALLOW_COPY_AND_ASSIGN(DX11CaptureRunner)
	public:
		DX11CaptureRunner(DX11* pDX11, DX11Image2D& image);
		virtual ~DX11CaptureRunner();
		void					SetConfig(const TinyString& className, const TinyString& exeName, Closure&& callback);
		void					SetInterval(UINT64 interval);
		void					SetFocusCPU(BOOL bFocusCPU);
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
		BOOL					Detour(const TinyString& className, const TinyString& exeName, BOOL bSafe = TRUE);
		BOOL					OpenEvents();
		void					CloseEvents();
		static BOOL CALLBACK	EnumWindow(HWND hwnd, LPARAM lParam);
	private:
		volatile BOOL			m_bCapturing;
		volatile BOOL			m_bActive;
		BOOL					m_bFocusCPU;
		BYTE*					m_textures[2];
		UINT64					m_interval;
		DX11*					m_pDX11;
		TinyLock				m_lock;
		WNDINFO					m_target;
		GameInject				m_injector;
		DX11Image2D&			m_image2D;
		TinyMutex				m_mutes[2];
		TinySize				m_size;
		TinyEvent				m_start;
		TinyEvent				m_stop;
		TinyEvent				m_sourceReady;
		TinyEvent				m_targetReady;
		TinyString				m_szClass;
		TinyString				m_szEXE;
		TinySharedMemory		m_hookDATA;
		TinySharedMemory		m_textureDATA;
		Closure					m_callback;
	};
}


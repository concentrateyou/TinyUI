#pragma once
#include "DXFramework.h"
#include "DX10Texture2D.h"
#include "DX10Image2D.h"
using namespace TinyFramework::IO;

namespace DXFramework
{
	class DX10CaptureRunner
	{
		DISALLOW_COPY_AND_ASSIGN(DX10CaptureRunner)
	public:
		DX10CaptureRunner(DX10* pDX10, DX10Image2D& image);
		virtual ~DX10CaptureRunner();
		void					SetConfig(const TinyString& className, const TinyString& exeName);
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
		DX10*					m_pDX10;
		TinyLock				m_lock;
		WNDINFO					m_target;
		GameInject				m_injector;
		DX10Image2D&			m_image2D;
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
	};
}


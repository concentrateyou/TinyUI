#pragma once
#include "DXFramework.h"
#include "DX10Texture2D.h"
#include "DX10Image2D.h"
using namespace TinyFramework::IO;

namespace DXFramework
{
	class DX10CaptureRunner : public TinyThread
	{
	public:
		DX10CaptureRunner(DX10& dx10, DX10Image2D& image);
		virtual ~DX10CaptureRunner();
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
		BOOL					Detour(const TinyString& className, const TinyString& exeName, const TinyString& dllName);
		BOOL					CreateEvents();
		BOOL					OpenEvents();
		void					CloseEvents();
		static BOOL CALLBACK	EnumWindow(HWND hwnd, LPARAM lParam);
	private:
		TinySize				m_size;
		BOOL					m_bCapturing;
		DX10&					m_dx10;
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
		DX10Image2D&			m_image;
		TinySharedMemory		m_captureMemory;
		TinySharedMemory		m_textureMemery;
	};
}


#pragma once
#include "DXFramework.h"
#include "DX11Texture2D.h"
#include "DX11Image.h"
#include "DX11Duplicator.h"
using namespace TinyUI::IO;

namespace DXFramework
{
	class DX11DuplicatorTask : public TinyTaskBase
	{
	public:
		DX11DuplicatorTask(DX11* pDX11, DX11Image& image);
		virtual ~DX11DuplicatorTask();
		void					SetConfig(const TinyString& exeName, const TinyString& dllName);
		BOOL					Submit();
		BOOL					Close(DWORD dwMS = INFINITE) OVERRIDE;
		SharedCaptureDATA*		GetSharedCaptureDATA();
		SharedTextureDATA*		GetSharedTextureDATA(DWORD dwSize);
		BYTE*					GetSharedTexture(DWORD dwSize);
	private:
		void					OnMessagePump();
		PROCESSINFO				GetPROCESSINFO();
		void					Tick();
		BOOL					BeginCapture();
		BOOL					EndCapture();
		BOOL					AttemptCapture(const TinyString& exeName, const TinyString& dllName);
		BOOL					CreateEvents();
		BOOL					OpenEvents();
		void					CloseEvents();
	private:
		TinySize				m_size;
		BOOL					m_bCapturing;
		DX11*					m_pDX11;
		TinyEvent				m_start;
		TinyEvent				m_stop;
		TinyEvent				m_ready;
		TinyEvent				m_exit;
		TinyEvent				m_close;
		PROCESSINFO				m_targetProcess;
		TinyString				m_className;
		TinyString				m_exeName;
		TinyString				m_dllName;
		DX11Image&				m_image;
		TinySharedMemory		m_captureMemory;
		TinySharedMemory		m_textureMemery;
	};
}



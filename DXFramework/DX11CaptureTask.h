#pragma once
#include "DXFramework.h"
#include "DX11Texture.h"
#include "DX11Image.h"
using namespace TinyUI::IO;

namespace DXFramework
{
	class DX11CaptureTask : public TinyTask
	{
	public:
		DX11CaptureTask(DX11* pDX11, TinyTaskPool* pWorks);
		virtual ~DX11CaptureTask();
		DX11Image*			GetTexture();
		BOOL				Submit();
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
		BOOL				m_bCapturing;
		DX11*				m_pDX11;
		TinyEvent			m_start;
		TinyEvent			m_stop;
		TinyEvent			m_ready;
		TinyEvent			m_exit;
		TinyEvent			m_close;
		HANDLE				m_hTask;
		WNDINFO				m_targetWND;
		SharedTexture		m_texture;
		TinySharedMemory	m_memory;
	};
}


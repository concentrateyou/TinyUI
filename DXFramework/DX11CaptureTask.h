#pragma once
#include "DXFramework.h"
#include "DX11Texture.h"
#include "DX11Image.h"
using namespace TinyUI::IO;

namespace DXFramework
{
	class DX11CaptureTask
	{
	public:
		DX11CaptureTask(DX11* pDX11);
		~DX11CaptureTask();
		void		Tick();
		DX11Image*	GetTexture();
		BOOL		Wait(DWORD dwTime);
	private:
		BOOL		BeginCapture();
		BOOL		EndCapture();
		BOOL		AttemptCapture(const TinyString& className, const TinyString& exeName, const TinyString& dllName);
		DWORD		InitializeEvent();
		SharedCaptureDATA* GetSharedCapture();
		WNDINFO	GetWNDINFO();
		static BOOL CALLBACK EnumWindow(HWND hwnd, LPARAM lParam);
		static DWORD MessageLoop(LPVOID lpUnused);
	private:
		DX11*				m_pDX11;
		TinyEvent			m_start;
		TinyEvent			m_stop;
		TinyEvent			m_ready;
		TinyEvent			m_exit;
		TinyEvent			m_close;
		WNDINFO				m_targetWND;
		SharedTexture		m_texture;
		TinySharedMemory	m_memory;
		HANDLE				m_hTask;
		BOOL				m_bCapturing;
	};
}


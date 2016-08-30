#pragma once
#include "DXFramework.h"
#include "DX11Texture.h"
#include "DX11Image.h"
using namespace TinyUI::IO;

namespace DXFramework
{
	class DX11CaptureSource
	{
	public:
		DX11CaptureSource();
		~DX11CaptureSource();
	public:
		BOOL		BeginCapture(const DX11& dx11);
		BOOL		EndCapture();
		void		Tick(const DX11& dx11);
		DX11Image*	GetTexture();
		BOOL		AttemptCapture(const DX11& dx11,const TinyString& className, const TinyString& exeName, const TinyString& dllName);
	private:
		DWORD	InitializeSignal();
		SharedCaptureDATA* GetSharedCapture();
		WNDINFO	GetWNDINFO();
		static BOOL CALLBACK EnumWindow(HWND hwnd, LPARAM lParam);
	private:
		TinyEvent			m_start;
		TinyEvent			m_stop;
		TinyEvent			m_ready;
		TinyEvent			m_exit;
		WNDINFO				m_targetWND;
		BOOL				m_bCapturing;
		SharedTexture		m_texture;
		TinySharedMemory	m_memory;
	};
}


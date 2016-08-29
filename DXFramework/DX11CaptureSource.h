#pragma once
#include "DXFramework.h"
#include "DX11Texture.h"
using namespace TinyUI::IO;

namespace DXFramework
{
	class DX11CaptureSource
	{
	public:
		DX11CaptureSource();
		~DX11CaptureSource();
	public:
		BOOL	BeginCapture(const DX11& dx10, const TinyString& processName, const TinyString& dll);
		BOOL	EndCapture();
		void	Tick(const DX11& dx10);
	private:
		BOOL	Initialize(const TinyString& processName);
		DWORD	FindProcess(const TinyString& processName);
		BOOL	FindWindow(const TinyString& processName);
		SharedCapture* GetSharedCapture();
		static BOOL CALLBACK EnumWindow(HWND hwnd, LPARAM lParam);
	private:
		TinyEvent			m_begin;
		TinyEvent			m_end;
		TinyEvent			m_ready;
		TinyEvent			m_exit;
		WNDINFO				m_targetWND;
		BOOL				m_bCapturing;
		BOOL				m_bInject;
		SharedTexture		m_textureCapture;
		TinySharedMemory	m_memory;
	};
}


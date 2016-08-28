#pragma once
#include "DXFramework.h"
#include "DX10Texture.h"

namespace DXFramework
{
	class DX10CaptureSource
	{
	public:
		DX10CaptureSource();
		~DX10CaptureSource();
	public:
		BOOL	BeginCapture(const TinyString& processName);
		BOOL	EndCapture();
		void	Tick(FLOAT fSeconds);
		BOOL	Render(const D3DXVECTOR2 &pos, const D3DXVECTOR2 &size);
		BOOL	Save(const CHAR* pzFile);
	private:
		BOOL	Initialize(const TinyString& processName);
		DWORD	FindProcess(const TinyString& processName);
		BOOL	FindWindow(const TinyString& processName);
		SharedCapture* GetSharedCapture();
		static BOOL CALLBACK EnumWindow(HWND hwnd, LPARAM lParam);
	private:
		TinyEvent					m_eventBegin;
		TinyEvent					m_eventEnd;
		TinyEvent					m_eventReady;
		TinyEvent					m_eventExit;
		WNDINFO						m_targetWND;
		BOOL						m_bCapturing;
		BOOL						m_bInject;
		SharedTextureCapture		m_textureCapture;
		IO::TinySharedMemory		m_sharedMemory;
	};
}


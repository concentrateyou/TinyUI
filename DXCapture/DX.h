#pragma once
#include "DXCommon.h"

namespace DXCapture
{
	LRESULT CALLBACK DXCbtFilter(INT code, WPARAM wParam, LPARAM lParam);
	class DX
	{
	public:
		DX();
		virtual ~DX();
		BOOL Initialize();
		void Uninitialize();
		BOOL SetWindowsHook();
		void UnhookWindowsHook();
		SharedCaptureDATA* GetSharedCaptureDATA();
		SharedTextureDATA* GetSharedTextureDATA();
	private:
		BOOL BuildEvents();
	public:
		HHOOK					m_hhk;
		TinyEvent				m_start;
		TinyEvent				m_stop;
		TinyEvent				m_ready;
		TinyEvent				m_exit;
		IO::TinySharedMemory	m_memery;
		IO::TinySharedMemory	m_textureMemery;
	};
	SELECTANY extern DX g_dx;
}

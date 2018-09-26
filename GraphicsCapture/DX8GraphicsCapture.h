#pragma once
#include "DX.h"

namespace GraphicsCapture
{
	/// <summary>
	/// XX8纹理数据捕获
	/// </summary>
	class DX8GraphicsCapture
	{
		DISALLOW_COPY_AND_ASSIGN(DX8GraphicsCapture)
	public:
		DX8GraphicsCapture(DX& dx);
		~DX8GraphicsCapture();
		BOOL Initialize(HWND hWND);
		BOOL Draw(LPVOID pThis);
		BOOL Setup(LPVOID pThis);
		BOOL DX8CPUHook(LPVOID pThis);
		void Reset();
	private:
		void OnMessagePump();
	public:
		LPVOID							m_currentPointer;
		volatile BOOL					m_bCapturing;
		volatile BOOL					m_bActive;
		volatile INT32					m_current;
		DWORD							m_dwCopy;
		DWORD							m_d3dFormat;
		DXGI_FORMAT						m_dxgiFormat;
		LPBYTE							m_textures[2];
		LPVOID							m_captures[NUM_BUFFERS];
		TinyWorker						m_captureTask;
		TinyEvent						m_copy;
		TinyEvent						m_close;
		TinyDetour						m_dX8EndScene;
		TinyDetour						m_dX8Present;
		TinyDetour						m_dX8Reset;
		TinyDetour						m_dX8Release;
		HMODULE							m_hD3D8;
		DX&								m_dx;
	};
	SELECTANY extern DX8GraphicsCapture g_dx8(g_dx);
}


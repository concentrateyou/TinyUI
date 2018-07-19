#pragma once
#include "DX.h"

namespace GraphicsCapture
{
#define NUM_BUFFERS 3
	/// <summary>
	/// XX8�������ݲ���
	/// </summary>
	class DX8GraphicsCapture
	{
		DISALLOW_COPY_AND_ASSIGN(DX8GraphicsCapture)
	public:
		DX8GraphicsCapture(DX& dx);
		~DX8GraphicsCapture();
		BOOL Initialize(HWND hWND);
		BOOL Render(LPVOID pThis);
		BOOL Setup(LPVOID pThis);
		BOOL DX8CPUHook(LPVOID pThis);
		void Reset();
	private:
		void OnMessagePump();
	public:
		LPVOID							m_currentPointer;
		SharedCaptureDATA				m_captureDATA;
		BOOL							m_bCapturing;
		BOOL							m_bTextures;
		DWORD							m_d3dFormat;
		DXGI_FORMAT						m_dxgiFormat;
		IO::TinyThread					m_captureTask;
		LPBYTE							m_textures[2];
		LPVOID							m_captures[NUM_BUFFERS];
		UINT							m_currentCapture;
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
